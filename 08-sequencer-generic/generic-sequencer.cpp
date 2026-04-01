#include <atomic>
#include <chrono>
#include <cstddef>
#include <print>
#include <queue>
#include <ratio>
#include <string>
#include <thread>

#include "xpto/semaphore.hpp"
#include "xpto/syslog.hpp"
#include "xpto/thread.hpp"

using freq_t = size_t;

struct service {
  using duration_t = std::chrono::duration<size_t, std::nano>;

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
using queue_t = std::priority_queue<
    queue_element_t, std::vector<queue_element_t>,
    std::greater<queue_element_t>>;

const xpto::syslogger logger{""};

int main() {
  auto services = std::array<service, 3>{
    service{"t1", 10},
    service{"t2", 3},
    service{"t3", 1},
  };

  queue_t queue;
  service::duration_t elapsed{0};

  for (auto&& x : services) {
    x.t = xpto::thread({}, [&]() {
      while (!x.abort) {
        x.sem.wait();
        if (x.abort) break;
        ++x.cycles;
        logger.debug(
            "start: {} @ {}", x.name,
            seconds_float_t(elapsed));
      }
      logger.debug("done: {} ", x.name);
    });
    queue.emplace(x.phase, &x);
  }

  logger.debug("queue has {} elements", queue.size());


  std::chrono::steady_clock stc;
  auto t1 = stc.now();

  using namespace std::chrono_literals;
  do { // NOLINT
    auto& top = queue.top();
    auto& x = *top.second;
    auto rem = top.first - elapsed;
    if (rem > 0s) {
      std::this_thread::sleep_for(rem);
      elapsed = top.first;
    }
    logger.debug("signalling {} @ {}", x.name, seconds_float_t{elapsed});
    x.sem.post();
    queue.pop();
    queue.emplace(elapsed + x.period(), &x);
  } while (elapsed < 3s);

  auto clock_elapsed = stc.now() - t1;

  logger.debug("elapsed: {} clock_elapsed: {} diff {}",
    elapsed,
    clock_elapsed,
      milliseconds_float_t{clock_elapsed-elapsed});

  for (auto&& x : services) {
    x.abort = true;
    x.sem.post();
  }
}
