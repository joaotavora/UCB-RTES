#include <thread>
#include <print>
#include <array>

static constexpr size_t num_threads{64};

int main(int argc, char* argv[]) {

  std::array<std::thread, num_threads> threads;
  size_t i =0;
  for (auto &t : threads) {
    t = std::thread([tid=i++]() {
      size_t sum{};
      for (int i = 1; i<=tid; ++i) sum += i;
      std::println("Thread[{}] sum[1..{}]={}", tid, tid, sum);
    });
  }
  for (auto &t : threads) t.join();
  std::println("Done!");
  return 0;
}
