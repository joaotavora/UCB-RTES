#include <chrono>
#include <cstddef>
#include <numeric>
#include <print>
#include <ratio>
#include <string>
#include <map>

#include "xpto/semaphore.hpp"
#include "xpto/syslog.hpp"
#include "xpto/thread.hpp"

using freq_t = size_t;

struct service {
  std::string name;
  freq_t frequency;
  using duration_t = std::chrono::duration<size_t, std::nano>;
  auto constexpr period() const { return duration_t{std::giga::num / frequency}; }
  size_t cycles{};
  xpto::thread t{};
  xpto::sem sem{name + "sem"};
  bool abort{};
};

struct step {
  service::duration_t wait{};
  std::vector<service*> services{};
};

using sequence_t = std::vector<step>;

constexpr auto as_seconds(auto duration) {
  return std::chrono::duration_cast<std::chrono::duration<double>>(duration);
}

sequence_t sequence(std::span<service> in) {
  sequence_t retval;
  std::map<service::duration_t, std::vector<service*>> map;

  using namespace std::chrono_literals;
  auto hyperperiod = std::accumulate(
      in.begin(), in.end(), service::duration_t{1},  //
      [](service::duration_t acc,
         auto&& s) -> std::chrono::duration<size_t, std::nano> {
        auto period = s.period();
        std::println("Period of {} is {}", s.name, s.period());
        auto ccount = std::lcm(acc.count(), s.period().count());
        return service::duration_t{ccount};
      });

  std::println("Hyperperiod is {}", as_seconds(hyperperiod));

  for (auto&& s : in) {
    auto period = s.period();
    for (service::duration_t i{0}; i < hyperperiod; i+= period) {
      map[i].push_back(&s);
    }
  }

  std::vector<std::pair<service::duration_t, std::vector<service*>>> hmmm;

  service::duration_t last{0};
  for (auto&& x : map) {
    retval.emplace_back(x.first - last, x.second);
    last = x.first;
  }
  return retval;
}




const xpto::syslogger logger{""};

int main() {

  auto services = std::array<service, 3>{
    service{"bla", 30},
    service{"bla", 3},
    service{"bla", 1},
  };

  auto const myseq = sequence(services);

  for (auto&& x : myseq) {
    std::print("{} -> [", as_seconds(x.wait));
    auto sep = "";
    for (auto&& y : x.services) {
      std::print("{}{}", sep, y->name);
      sep = ", ";
    }
    std::println("]");
  }
  
  std::chrono::steady_clock stc;

  // services.emplace_back(std::make_unique<service>("s1", 10));
  // services.emplace_back(std::make_unique<service>("s2", 50));

  
}
