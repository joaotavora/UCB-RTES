#include "xpto/thread.hpp"
#include "xpto/rtes-misc.hpp"
#include <chrono>
#include <print>

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
  xpto::install_rt_scheduler();

  using attributes = xpto::thread::attributes;
  auto rt_max_prio = sched_get_priority_max(SCHED_FIFO);
  attributes attr;

  auto i  = 0;
  xpto::thread t1{{
    .policy = attributes::FIFO,
    .prio = rt_max_prio - i - 1,
  }, []() {
    std::chrono::high_resolution_clock rt;
    auto t1 = rt.now();
    FIB_TEST(230, 10000); // around 10 us on a RP5
    auto elapsed = rt.now() - t1;
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
    std::println("This work took {}", us);
  }};
}
