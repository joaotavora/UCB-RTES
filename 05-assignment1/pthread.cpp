#include <sys/syslog.h>
#include <syslog.h>

#include <array>
#include <cstdio>
#include <format>
#include <print>
#include <stdexcept>
#include <string>

#include "xpto/auto.hpp"
#include "xpto/thread.hpp"  // IWYU pragma: keep
#include "xpto/orlose.hpp"

std::string exec(const char* cmd) {
  std::array<char, 128> buffer{};
  std::string result;
  FILE* pipe{};
  AUTO(fclose(pipe));  // NOLINT
  pipe = xpto::or_lose(popen(cmd, "r"));
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) !=
         nullptr) {
    result += buffer.data();
  }
  return result;
}

namespace xpto {
template <typename... Args>
void syslog(unsigned int prio, std::format_string<const Args&...> fmt,
            const Args&... args) {
  ::syslog(prio, "[COURSE:1][ASSIGNMENT:1] %s\n",
           std::format(fmt, args...).c_str());
}
}  // namespace xpto
int main(int argc, char* argv[]) {
  // std::span<char*> args{argv, static_cast<size_t>(argc)};
  // openlog(args[0], 0, 0);

  xpto::syslog(LOG_INFO, "{}", exec("uname -a"));
  xpto::syslog(LOG_INFO, "Hello World from Main!");
  xpto::thread([]() { xpto::syslog(LOG_INFO, "Hello World from Thread!"); });
}
