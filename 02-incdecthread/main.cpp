#include <thread>
#include <print>

int gsum = 0; // NOT safe  (NOLINT)
static constexpr int count = 10000;

int main() {
  std::thread t1{[&]() {
    for (auto i = 0; i < count ; ++i) {
      gsum++;
      // println("Increment thread idx={} gsum={}", std::this_thread::get_id(), gsum);
    }
  }};

  std::thread t2{[&]() {
    for (auto i = 0; i < count ; ++i) {
      gsum--;
      // println("Decrement thread idx={} gsum={}", std::this_thread::get_id(), gsum);
    }
  }};
  t1.join();
  t2.join();

  std::println("Finally gsum = {}", gsum);
  
}
