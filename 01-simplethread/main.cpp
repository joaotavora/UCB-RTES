#include <cstdio>
#include <array>

#include "xpto/pthread.hpp"

static constexpr size_t num_threads{64};
  // namespace xpto

int main(int argc, char* argv[]) {
  {
    auto fn = [](int tid) {
      size_t sum{};
      for (int i = 1; i <= tid; ++i) sum += i;
      printf("Thread[%d] sum[1..%d]=%lu\n", tid, tid, sum);
    };
    using thread_t = xpto::thread<decltype(fn)>;
    std::array<thread_t, num_threads> threads;

    // threads.reserve(num_threads);
    for (auto i = 0u; i < num_threads; ++i) {
      threads.at(i) = xpto::thread{fn, i};
    }
  }

  printf("Done!\n");
  return 0;
}
