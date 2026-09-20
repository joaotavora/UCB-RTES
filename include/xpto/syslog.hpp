#include <sys/syslog.h>
#include <syslog.h>

#include <array>
#include <cstddef>
#include <fmt/format.h>
#include <string>

namespace xpto {

class syslogger {
  std::string prefix_{};

 public:
  explicit syslogger(const std::string& prefix = "")
      : prefix_{prefix + " %s\n"} {}

  template <typename... Args>
  void syslog(
      int prio, fmt::format_string<const Args&...> fmt,
      const Args&... args) const {
    std::array<char, 1024> buf{};
    auto cap = static_cast<std::ptrdiff_t>(buf.size() - 1);
    auto res = fmt::format_to_n(buf.data(), cap, fmt, args...);
    *res.out = '\0';
    ::syslog(prio, prefix_.c_str(), buf.data());
  }

  template <typename... Args>
  void debug(fmt::format_string<const Args&...> fmt, const Args&... args) const {
    syslog(LOG_DEBUG, fmt, args...);
  }
};

}  // namespace xpto
