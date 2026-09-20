#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fmt/chrono.h>
#include <queue>
#include <ratio>
#include <string>
#include <thread>
#include <utility>
#include <vector>

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
  size_t releases{};  // sequencer thread's release counter
  xpto::sem sem{name + "sem"};
  xpto::thread t{};
  std::atomic<bool> abort{};
};

// 'elapsed' below is shared with the service threads; updating it must
// be lock-free, or the timing loop would pay for locks on every release.
static_assert(std::atomic<service::duration_t>::is_always_lock_free,
              "sequencer clock requires lock-free 64-bit atomics");

using seconds_float_t = std::chrono::duration<double>;
using milliseconds_float_t = std::chrono::duration<double, std::milli>;

using queue_element_t = std::pair<service::duration_t, service*>;

// Min-heap on deadline.  Ties break deterministically by service
// address order (std::less provides the total order that built-in
// pointer comparison doesn't).
struct earliest_deadline {
  bool operator()(queue_element_t const& a, queue_element_t const& b) const {
    if (a.first != b.first) return a.first > b.first;
    return std::less<service*>{}(b.second, a.second);
  }
};

using queue_t =
    std::priority_queue<queue_element_t, std::vector<queue_element_t>,
                        earliest_deadline>;

const xpto::syslogger logger{""};

int main() {
  auto services =
      std::array{service{"t1", 10}, service{"t2", 3}, service{"t3", 1}};

  // Reserve so the schedule never touches the heap, not even on the
  // first releases.
  std::vector<queue_element_t> storage;
  storage.reserve(services.size());
  queue_t queue{earliest_deadline{}, std::move(storage)};
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
    ++x.releases;
    // Absolute deadline from the phase: the division truncates each
    // deadline by at most 1 ns, but unlike adding a truncated period
    // on every release, the error never accumulates.
    auto next =
        x.phase + service::duration_t{static_cast<int64_t>(x.releases) *
                                      std::giga::num /
                                      static_cast<int64_t>(x.frequency)};
    queue.emplace(next, &x);
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
