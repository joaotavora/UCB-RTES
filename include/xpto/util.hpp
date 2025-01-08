#pragma once

#include <cstring>

#define CFUNCALL(func)                                            \
  if (auto res = (func); res != 0) {                              \
    throw std::runtime_error(std::string(#func) +                 \
                             " failed: " + std::strerror(errno)); \
  }
