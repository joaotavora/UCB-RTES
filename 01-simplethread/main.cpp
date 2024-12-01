#include <cstdio>
#include <array>
#include <thread>

static constexpr size_t num_threads{64};
  // namespace xpto

int main(int argc, char* argv[]) {
  std::array<std::thread, num_threads> threads;

  // threads.reserve(num_threads);
  for (auto i = 0u; i < num_threads; ++i) {
    threads.at(i) = std::thread{[](int tid) {
      size_t sum{};
      for (int i = 1; i <= tid; ++i) sum += i;
      printf("Thread[%d] sum[1..%d]=%lu\n", tid, tid, sum);
    }, i};
  }
  for (auto& t : threads) t.join();

  printf("Done!\n");
  return 0;
}
