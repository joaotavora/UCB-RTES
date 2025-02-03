#include <sched.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include <chrono>
#include <print>
#include <vector>

#include "xpto/thread.hpp"
#include "xpto/util.hpp"

// NOLINTBEGIN(*swappable*)
int FIB_TEST(unsigned int seqCnt, unsigned int iterCnt) {
  // NOLINTEND(*swappable*)
  int idx = 0, jdx = 1;
  unsigned int fib = 0, fib0 = 0, fib1 = 1;
  for (idx = 0; idx < iterCnt; idx++) {
    fib = fib0 + fib1;
    while (jdx < seqCnt) {
      fib0 = fib1;
      fib1 = fib;
      fib = fib0 + fib1;
      jdx++;
    }

    jdx = 0;
  }

  return idx;
}

int main() {
  std::println("This system has {} processors with {} available",
               get_nprocs_conf(), get_nprocs());
  std::println(
      "The test thread created will be SCHED_FIFO, is run with sudo and will be"
      "run on least busy core");

  auto rt_max_prio = sched_get_priority_max(SCHED_FIFO);
  auto rt_min_prio = sched_get_priority_min(SCHED_FIFO);
  std::println("rt_max_prio={}", rt_max_prio);
  std::println("rt_min_prio={}", rt_min_prio);

  xpto::dump_scheduler();

  // Now let's set some stuff for the main process and its single thread
  sched_param main_param{};
  xpto::or_lose(sched_getparam(gettid(), &main_param));
  std::println(
      "I'll now adjust the priority since it's {} right now (and that probably "
      "isn't acceptable for SCHED_FIFO)",
      main_param.sched_priority);
  main_param.sched_priority = rt_max_prio;
  xpto::or_lose(sched_setscheduler(gettid(), SCHED_FIFO, &main_param));
  std::println("New scheduler installed");
  xpto::dump_scheduler();

  constexpr size_t k_num_threads = 4;
  constexpr size_t k_sum_iterations = 1000;
  constexpr unsigned int k_seq_iterations = 250;
  constexpr unsigned int k_req_iterations = 10000000;

  std::vector<xpto::thread> threads{};
  std::chrono::high_resolution_clock clock{};
  for (auto i = 0; i < k_num_threads; ++i) {
    using attributes = xpto::thread::attributes;
    attributes attr{
        .policy = attributes::FIFO,
        .prio = rt_max_prio - i - 1,
    };
    threads.emplace_back(attr, [&clock, idx = i]() {
      size_t sum{};
      auto t1 = clock.now();
      for (auto i = 1; i < (idx + 1 * k_sum_iterations); i++) sum = sum + i;
      FIB_TEST(k_seq_iterations, k_req_iterations);
      auto elapsed = clock.now() - t1;
      auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed);
      auto subsecond = elapsed - seconds;

      std::println(
          "Thread idx={} ran {} sec and ({} msec {} microsec {} nsec) on core "
          "{}",
          idx, seconds,
          std::chrono::duration_cast<std::chrono::milliseconds>(subsecond),
          std::chrono::duration_cast<std::chrono::microseconds>(subsecond),
          std::chrono::duration_cast<std::chrono::nanoseconds>(subsecond),
          sched_getcpu());
    });
  }
}
