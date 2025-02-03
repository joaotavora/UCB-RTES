#include <chrono>
#include <print>
#include <vector>

#include "xpto/thread.hpp"

void print_scheduler(void) {
  int schedType = sched_getscheduler(getpid());

  switch (schedType) {
    case SCHED_FIFO:
      std::println("Pthread policy is SCHED_FIFO");
      break;
    case SCHED_OTHER:
      std::println("Pthread policy is SCHED_OTHER");
      break;
    case SCHED_RR:
      std::println("Pthread policy is SCHED_RR");
      break;
    default:
      std::println("Pthread policy is UNKNOWN");
  }
}
// use FIFO RT max priority attributes
const xpto::thread::attributes fifo_rt_max_prio_attrs{
    .affinity = {3},
    .stack_size = {},
    .policy = xpto::thread::attributes::FIFO,
};

int main() {
  {
    std::println("Main thread running on CPU={}", sched_getcpu());
    print_scheduler();
    std::vector<xpto::thread> threads;

    xpto::thread t{
        fifo_rt_max_prio_attrs, [&]() {
          std::println("Starter thread running on CPU={}", sched_getcpu());
          print_scheduler();

          for (auto i = 0; i < 64; i++) {
            threads.emplace_back(fifo_rt_max_prio_attrs, [i]() {
              auto start = std::chrono::system_clock::now();
              long sum = 0;

              for (auto iterations = 0; iterations < (1000000); iterations++) {
                sum = 0;
                for (auto j = 1; j <= i; j++) sum += j;
              }
              auto elapsed = std::chrono::system_clock::now() - start;

              std::printf(
                  "Thread idx=%d, sum[0...%d]=%ld, running on CPU=%d, took %ld\n",
                  i, i, sum, sched_getcpu(), elapsed.count());
            });
          }
        }};
    std::println("main thread: Joining...");
  }
}
