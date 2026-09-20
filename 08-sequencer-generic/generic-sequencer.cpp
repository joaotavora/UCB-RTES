#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <queue>
#include <ratio>
#include <string>
#include <thread>

#include "xpto/semaphore.hpp"
#include "xpto/syslog.hpp"
#include "xpto/thread.hpp"

using freq_t = size_t;

struct service {
  using duration_t = std::chrono::duration<int64_t, std::nano>;

  std::string name;
  freq_t frequency;
  duration_t phase{};

  size_t cycles{};
  xpto::sem sem{name + "sem"};
  xpto::thread t{};
  std::atomic<bool> abort{};

  duration_t constexpr period() const {
    return duration_t{std::giga::num / frequency};
  }
};

using seconds_float_t = std::chrono::duration<double>;
using milliseconds_float_t = std::chrono::duration<double, std::milli>;

using queue_element_t = std::pair<service::duration_t, service*>;
using queue_t =
    std::priority_queue<queue_element_t, std::vector<queue_element_t>,
                        std::greater<queue_element_t>>;

const xpto::syslogger logger{""};

int main() {
  auto services =
      std::array{service{"t1", 10}, service{"t2", 3}, service{"t3", 1}};

  queue_t queue;
  std::atomic<service::duration_t> elapsed{service::duration_t{0}};

  for (auto&& x : services) {
    x.t = xpto::thread({}, [&]() {
      while (!x.abort) {
        x.sem.wait();
        if (x.abort) break;
        ++x.cycles;
        logger.debug("start: {} @ {}", x.name,
                     seconds_float_t(
                         elapsed.load(std::memory_order_relaxed)));
      }
      logger.debug("done: {} ", x.name);
    });
    queue.emplace(x.phase, &x);
  }

  logger.debug("queue has {} elements", queue.size());

  std::chrono::steady_clock stc;
  auto t1 = stc.now();

  using namespace std::chrono_literals;
  do {  // NOLINT
    auto& top = queue.top();
    auto& x = *top.second;
    auto rem = top.first - elapsed.load(std::memory_order_relaxed);
    if (rem > 0s) {
      std::this_thread::sleep_for(rem);
      elapsed.store(top.first, std::memory_order_relaxed);
    } else if (rem < 0s) {
      logger.debug("deadline for {} missed by {}", x.name,
                   seconds_float_t{-rem});
    }
    logger.debug("signalling {} @ {}", x.name,
                 seconds_float_t{elapsed.load(std::memory_order_relaxed)});
    x.sem.post();
    queue.pop();
    queue.emplace(elapsed.load(std::memory_order_relaxed) + x.period(), &x);
  } while (elapsed.load(std::memory_order_relaxed) < 3s);

  auto clock_elapsed = stc.now() - t1;
  auto el = elapsed.load(std::memory_order_relaxed);

  logger.debug("elapsed: {} clock_elapsed: {} diff {}", el, clock_elapsed,
               milliseconds_float_t{clock_elapsed - el});

  for (auto&& x : services) {
    x.abort = true;
    x.sem.post();
  }
}
