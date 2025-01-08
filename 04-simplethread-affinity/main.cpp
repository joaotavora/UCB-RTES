#include <thread>
#include <print>
#include "xpto/thread.hpp"

int main( ) {
  using namespace std::chrono_literals;
  {
  xpto::thread t{[](){
    std::println("second thread: hello, I'm a thread?");
    std::this_thread::sleep_for(2s);
    std::println("second thread: and now I'm done?");
  }};
  std::println("main thread: Joining...");
  }
  
}
