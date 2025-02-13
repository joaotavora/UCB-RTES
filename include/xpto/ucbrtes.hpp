#pragma once

#include <pthread.h>
#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <chrono>
#include <cstdio>
#include <format>
#include <print>

#include "xpto/orlose.hpp"

namespace xpto::concepts {
template <class T>
struct is_duration_t : std::false_type {};

template <class Rep, class Period>
struct is_duration_t<std::chrono::duration<Rep, Period>> : std::true_type {};

template <typename T>
concept duration = is_duration_t<std::remove_cvref_t<T>>::value;

static_assert(
    is_duration_t<std::chrono::high_resolution_clock::duration>::value);

}  // namespace xpto::concepts

namespace xpto {

void dump_scheduler(std::FILE* stream = stdout) {
  int scope{};
  pthread_attr_t main_attr;
  syscall(SYS_sched_getattr, gettid(), &main_attr,
      sizeof(main_attr), 0);

  auto pol = sched_getscheduler(gettid());
  xpto::or_lose(pthread_attr_getscope(&main_attr, &scope));

  std::println(
      stream, "tid:{} policy:{} scope:{}", gettid(),
      [&]() {
        switch (pol) {
          case SCHED_FIFO:
            return "SCHED_FIFO";
          case SCHED_OTHER:
            return "SCHED_OTHER";
          case SCHED_RR:
            return "SCHED_RR";
          default:
            return "(unknown)";
        }
      }(),
      [&]() {
        switch (scope) {
          case PTHREAD_SCOPE_SYSTEM:
            return "PTHREAD_SCOPE_SYSTEM";
          case PTHREAD_SCOPE_PROCESS:
            return "PTHREAD_SCOPE_PROCESS";
          default:
            return "(unknown)";
        }
      }());
}

void install_rt_scheduler(int prio = sched_get_priority_max(SCHED_FIFO)) {
  // Now let's set some stuff for the main process and its single thread
  sched_param main_param{};
  xpto::or_lose(sched_getparam(gettid(), &main_param));
  // adjust priority to something acceptable to SCHED_FIFO (it's probably 0)
  main_param.sched_priority = prio;
  xpto::or_lose(sched_setscheduler(gettid(), SCHED_FIFO, &main_param));
  std::println("Installed RT scheduler on tid: {}", gettid());
  xpto::dump_scheduler();
}

// NOLINTBEGIN(*swappable*)
int FIB_TEST(unsigned int seqCnt, unsigned int iterCnt) {
  // NOLINTEND(*swappable*)
  int idx = 0, jdx = 1;
  // need volatile otherwise compiler gets rid of this somehow
  volatile unsigned int fib = 0, fib0 = 0, fib1 = 1;
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

auto work_for(concepts::duration auto dur, std::string blurb = {}) {
  std::chrono::high_resolution_clock rt;
  auto f = []() { FIB_TEST(15, 5000); };
  f();  // warm cache
  auto t1 = rt.now();
  f();
  auto elapsed = rt.now() - t1;
  auto iterations = dur / elapsed;

  if (!blurb.size()) blurb = std::format("W{}", dur);
  std::println("{} need about {} iterations for {}", blurb, iterations, dur);
  return [f, blurb, iterations,
          i = 0](std::chrono::high_resolution_clock::time_point start) mutable {
    std::chrono::high_resolution_clock rt;
    ++i;
    auto t1 =
        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
            rt.now() - start);
    std::println("{} start {} @ {} on core {}", blurb, i, t1, sched_getcpu());
    for (auto i = 0; i < iterations; ++i) f();
    auto t2 =
        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
            rt.now() - start);
    std::println("{} end   {} @ {} on core {}", blurb, i, t2, sched_getcpu());
  };
}

}  // namespace xpto
