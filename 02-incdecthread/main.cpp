#include <atomic>
#include <fmt/core.h>

#include "xpto/thread.hpp"

int gsum = 0;  // NOT safe (NOLINT)
std::atomic<int> agsum{0};  // safe (NOLINT)
static constexpr int count = 10000;

int main() {
  xpto::thread t1{[&]() {
    for (auto i = 0; i < count; ++i) {
      gsum++;
      agsum++;
    }
  }};

  xpto::thread t2{[&]() {
    for (auto i = 0; i < count; ++i) {
      gsum--;
      agsum--;
    }
  }};

  fmt::println("Finally gsum = {} agsum = {}", gsum, agsum.load());
}
