#include <sys/syslog.h>
#include <syslog.h>

#include <format>

namespace xpto {

class syslogger {
  std::string prefix_{};

 public:
  explicit syslogger(const std::string& prefix = "")
      : prefix_{prefix + " %s\n"} {}

  template <typename... Args>
  void syslog(
      unsigned int prio, std::format_string<const Args&...> fmt,
      const Args&... args) const {
    ::syslog(prio, prefix_.c_str(), std::format(fmt, args...).c_str());
  }

  template <typename... Args>
  void debug(std::format_string<const Args&...> fmt, const Args&... args) const {
    syslog(LOG_DEBUG, fmt, args...);
  }
};

}  // namespace xpto
