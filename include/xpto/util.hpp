#pragma once

#include <cstring>
#include <source_location>
#include <stdexcept>
#include <string>
#include <type_traits>

#define ZCALL_OR_LOSE(func)                                     \
  if (auto res = (func); res != 0) {                            \
    if (errno == 0) errno = res;                                \
    throw std::runtime_error(std::string(#func) + " failed (" + \
                             std::to_string(res) +              \
                             "): " + std::strerror(errno));     \
  }

#define PCALL_OR_LOSE(func)                                         \
  [&]() {                                                           \
    if (auto res = (func); res == nullptr) {                        \
      throw std::runtime_error(std::string(#func) +                 \
                               " failed: " + std::strerror(errno)); \
    }                                                               \
    return res;                                                     \
  }()

namespace xpto {
// Base template for general types
template <typename T>
T or_lose(T value, std::source_location loc = std::source_location::current()) {
  if constexpr (std::is_pointer_v<T>) {
    // Specialization for pointer types: check if pointer is null
    if (value == nullptr) {
      throw std::runtime_error("Oops: " + std::string(loc.file_name()) + ":" +
                               std::to_string(loc.line()) + " (" +
                               std::string(loc.function_name()) +
                               "): " + std::strerror(errno));
    }
  } else if constexpr (std::is_integral_v<T>) {
    // Specialization for integral types: check if value is non-zero
    if (value != 0) {
      if (errno == 0) errno = value;
      throw std::runtime_error(
          "Oops: " + std::string(loc.file_name()) + ":" +
          std::to_string(loc.line()) + " (" + std::string(loc.function_name()) +
          "): (" + std::to_string(value) + ")" + std::strerror(errno));
    }
  } else {
    static_assert(false, "Unsupported type.");
  }

  return value;
}
}  // namespace xpto
