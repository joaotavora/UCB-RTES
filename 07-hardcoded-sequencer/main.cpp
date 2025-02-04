#include "xpto/thread.hpp"
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
  std::chrono::high_resolution_clock rt;
  auto t1 = rt.now();
  FIB_TEST(70, 10000);
  auto elapsed = rt.now() - t1;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
  std::println("This work took {}", ms);
}
