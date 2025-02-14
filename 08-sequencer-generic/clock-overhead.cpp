#include <chrono>
#include <ctime>
#include <print>

namespace xpto {

class fast_monotonic_clock {
public:
  using duration = std::chrono::nanoseconds;
  using rep = duration::rep;
  using period = duration::period;
  using time_point = std::chrono::time_point<fast_monotonic_clock>;

static constexpr bool is_steady = true;

static time_point now() noexcept {
  timespec t; // NOLINT(*init*)
  const auto result = clock_gettime(clock_id(), &t);
  return time_point{convert(t)};
}

static duration get_resolution() noexcept {
  timespec t; // NOLINT(*init*)
  const auto result = clock_getres(clock_id(), &t);
  return convert(t);
}

private:
  static clockid_t clock_id() {
    static clockid_t the_clock = test_coarse_clock();
    return the_clock;
  }
static clockid_t test_coarse_clock() {
  timespec t; // NOLINT(*init*)
  if (clock_gettime(CLOCK_MONOTONIC_COARSE, &t) == 0) {
    return CLOCK_MONOTONIC_COARSE;
  } else {
    return CLOCK_MONOTONIC;
  }
};
static duration convert(const timespec& t) {
  return std::chrono::seconds(t.tv_sec) + std::chrono::nanoseconds(t.tv_nsec);
}
};

}  // namespace xpto

int main() {
  std::chrono::high_resolution_clock hrc;
  std::chrono::steady_clock stc;
  xpto::fast_monotonic_clock fmc;

  {
    auto t1 = hrc.now();
    for (auto i = 0; i < 1000000; ++i) hrc.now();
    std::println("std::chrono::high_resolution_clock took {}", hrc.now() - t1);
  }

  {
    auto t1 = hrc.now();
    for (auto i = 0; i < 1000000; ++i) stc.now();
    std::println("std::chrono::steady_clock took {}", hrc.now() - t1);
  }

  {
    auto t1 = hrc.now();
    for (auto i = 0; i < 1000000; ++i) fmc.now();
    std::println("xpto::fast_monotonic_clock took {}", hrc.now() - t1);
  }
}
