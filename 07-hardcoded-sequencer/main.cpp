#include <chrono>
#include <print>
#include <thread>

#include "xpto/semaphore.hpp"
#include "xpto/thread.hpp"
#include "xpto/ucbrtes.hpp"

int main() {
  xpto::install_rt_scheduler();

  using namespace std::chrono_literals;
  auto work20 = xpto::work_for(20ms, "t20");
  auto work10 = xpto::work_for(10ms, "t10");
  bool abort_test = false;

  auto rt_max_prio = sched_get_priority_max(SCHED_FIFO);
  xpto::sem semf10{};
  xpto::sem semf20{};

  std::chrono::high_resolution_clock hrc;
  decltype(hrc)::time_point start{};
  using namespace std::chrono_literals;

  auto i = 0;
  xpto::thread t20{
    {
      .affinity = {3},
      .name = "t20",
      .policy = xpto::thread::attributes::FIFO,
      .prio = rt_max_prio - 2,
    },
    [&]() {
      std::println("t20 starting (sem val is {})", semf20.value());
      while (!abort_test) {
        semf20.wait();
        if (abort_test) break;
        work20(start);
      }
    }};

  xpto::thread t10{
    {
      .affinity = {3},
      .name = "t10",
      .policy = xpto::thread::attributes::FIFO,
      .prio = rt_max_prio - 1,
    },
    [&]() {
      std::println("t10 starting (sem val is {})", semf10.value());
      while (!abort_test) {
        semf10.wait();
        if (abort_test) break;
        work10(start);
      }
    }};

  xpto::thread sequencer{
      {.name = "sequencer",
       .policy = xpto::thread::attributes::FIFO,
       .prio = rt_max_prio - 0},
      [&]() {
        std::println("Starting!");
        start = hrc.now();
        auto event_time = 0ms;
        auto mark = [&]() {
          event_time = std::chrono::duration_cast<std::chrono::milliseconds>(
              hrc.now() - start);
          return event_time;
        };
        auto major_periods = 3;

        while (major_periods--) {
          // Basic sequence of releases after CI for 90% load
          //
          // S1: T1= 20, C1=10 msec
          // S2: T2= 50, C2=20 msec
          //
          // This is equivalent to a Cyclic Executive Loop where the major cycle
          // is 100 milliseconds with a minor cycle of 20 milliseconds, but here
          // we use pre-emption rather than a fixed schedule.
          //
          // Add to see what happens on edge of overload
          // T3=100, C3=10 msec -- add for 100% utility
          //
          // Use of usleep is not ideal, but is sufficient for predictable
          // response.
          //
          // To improve, use a real-time clock (programmable interval time with
          // an ISR) which in turn raises a signal (software interrupt) to a
          // handler that performs the release.
          //
          // Better yet, code a driver that direction interfaces to a hardware
          // PIT and sequences between kernel space and user space.
          //
          // Final option is to write all real-time code as kernel tasks, more
          // like an RTOS such as VxWorks.
          //

          // Simulate the C.I. for S1 and S2 and timestamp in log
          std::println("\n**** CI t={}", mark());
          semf10.post();
          semf20.post();

          std::this_thread::sleep_for(20ms);
          std::println("t={}", mark());
          semf10.post();

          std::this_thread::sleep_for(20ms);
          std::println("t={}", mark());
          semf10.post();

          std::this_thread::sleep_for(10ms);
          std::println("t={}", mark());
          semf20.post();

          std::this_thread::sleep_for(10ms);
          std::println("t={}", mark());
          semf10.post();

          std::this_thread::sleep_for(20ms);
          std::println("t={}", mark());
          semf10.post();

          std::this_thread::sleep_for(20ms);
        }
        semf20.post();
        semf10.post();
        abort_test = true;
      }};
}
