#include <print>
#include <thread>
#include <vector>

static constexpr size_t num_threads{64};

int main(int argc, char *argv[]) {
  std::vector<std::thread> threads;
  threads.reserve(num_threads);
  for (auto i = 0u; i < num_threads; ++i) {
    threads.emplace_back(
        [](int tid) {
          size_t sum{};
          for (int i = 1; i <= tid; ++i) sum += i;
          std::println("Thread[{}] sum[1..{}]={}", tid, tid, sum);
        },
        i++);
  }
  for (auto &t : threads) t.join();
  std::println("Done!");
  return 0;
}
