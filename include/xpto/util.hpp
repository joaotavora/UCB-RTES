#pragma once

#include <stdexcept>  // IWYU pragma: keep
#include <cstring>

#define CCALL_OR_LOSE(func)                                       \
  if (auto res = (func); res != 0) {                              \
    if (errno == 0) errno = res;                                  \
    throw std::runtime_error(std::string(#func) +                 \
       " failed (" + std::to_string(res) + "): " +                \
       std::strerror(errno));                                     \
  }
