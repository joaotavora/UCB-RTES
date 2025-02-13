#pragma once

#include <pthread.h>
#include <sched.h>
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
  int schedType{}, scope{};
  pthread_attr_t main_attr;

  schedType = sched_getscheduler(getpid());

  switch (schedType) {
    case SCHED_FIFO:
      std::println(stream, "Pthread Policy is SCHED_FIFO");
      break;
    case SCHED_OTHER:
      std::println(stream, "Pthread Policy is SCHED_OTHER");
      break;
    case SCHED_RR:
      std::println(stream, "Pthread Policy is SCHED_RR");
      break;
    default:
      std::println(stream, "Pthread Policy is UNKNOWN");
  }

  pthread_attr_getscope(&main_attr, &scope);

  if (scope == PTHREAD_SCOPE_SYSTEM)
    std::println(stream, "PTHREAD SCOPE SYSTEM");
  else if (scope == PTHREAD_SCOPE_PROCESS)
    std::println(stream, "PTHREAD SCOPE PROCESS");
  else
    std::println(stream, "PTHREAD SCOPE UNKNOWN");
}

void install_rt_scheduler() {
  auto rt_max_prio = sched_get_priority_max(SCHED_FIFO);
  // Now let's set some stuff for the main process and its single thread
  sched_param main_param{};
  xpto::or_lose(sched_getparam(gettid(), &main_param));
  std::println(
      "I'll now adjust the priority since it's {} right now (and that probably "
      "isn't acceptable for SCHED_FIFO)",
      main_param.sched_priority);
  main_param.sched_priority = rt_max_prio;
  xpto::or_lose(sched_setscheduler(gettid(), SCHED_FIFO, &main_param));
  std::println("New RT scheduler installed on current thread");
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
  std::println("I figure I need {} iterations for {}", iterations, dur);
  return [f, blurb, iterations,
          i = 0](std::chrono::high_resolution_clock::time_point start) mutable {
    std::chrono::high_resolution_clock rt;
    ++i;
    auto t1 =
      std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(rt.now() - start);
    std::println("{} start {} @ {} on core {}", blurb, i, t1, sched_getcpu());
    for (auto i = 0; i < iterations; ++i) f();
    auto t2 =
        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(rt.now() - start);
    std::println("{} end   {} @ {} on core {}", blurb, i, t2, sched_getcpu());
  };
}

}  // namespace xpto
