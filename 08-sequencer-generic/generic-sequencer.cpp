#include <chrono>
#include <cstddef>
#include <memory>
#include <numeric>
#include <print>
#include <ratio>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "xpto/semaphore.hpp"
#include "xpto/syslog.hpp"
#include "xpto/thread.hpp"

using freq_t = size_t;

struct service {
  std::string name;
  freq_t frequency;
  size_t cycles{};
  xpto::thread t{};
  xpto::sem sem{name + "sem"};
  bool abort{};

  using duration_t = std::chrono::duration<size_t, std::nano>;

  auto period() { return duration_t{std::giga::num / frequency}; }
};

using services_t = std::vector<std::unique_ptr<service>>;

const xpto::syslogger logger{""};

void sequence(services_t& services) {
  using namespace std::chrono_literals;

  auto elapsed = service::duration_t{};

  for (auto&& s : services) {
    s->t = xpto::thread({}, [&]() {
      while (!s->abort) {
        s->sem.wait();
        if (s->abort) break;
        ++s->cycles;
        logger.debug("start: {} @ {}", s->name,
            std::chrono::duration_cast<std::chrono::duration<double>>(elapsed));
      }
      logger.debug("done: {} ", s->name);
    });
  }

  auto quantum = std::accumulate(
      services.begin(), services.end(), 1'000'000'000ns,  //
      [](service::duration_t acc,
         auto&& s) -> std::chrono::duration<size_t, std::nano> {
        auto period = s->period();
        if (period > 1s)
          throw std::runtime_error("No support for frequencies less than 1Hz");
        auto ccount = std::gcd(acc.count(), s->period().count());
        return service::duration_t{ccount};
      });
  logger.debug("The gcd is {}", quantum);

  while (elapsed < 3s) {
    for (auto&& s : services) {
      if (elapsed % s->period() == 0s) {
        s->sem.post();
      }
    }
    std::this_thread::sleep_for(quantum);
    elapsed+=quantum;
  }

  for (auto&& s : services) {
    s->sem.post();
    s->abort = true;
  }
}

int main() {
  std::chrono::steady_clock stc;

  services_t services{};

  services.emplace_back(std::make_unique<service>("s1", 10));
  services.emplace_back(std::make_unique<service>("s2", 50));
  sequence(services);

  
}
