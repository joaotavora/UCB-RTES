#pragma once

#include "xpto/c_resource.hpp"
#include <semaphore.h>
#include <stdexcept>

namespace xpto {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
struct sem : private xpto::c_resource<::sem_open, ::sem_close, SEM_FAILED> {
#pragma GCC diagnostic pop
  using c_resource::c_resource;

  void wait() {
    if (auto res = sem_wait(*this); res < 0)
      throw std::runtime_error("sem_wait");
  }

  void post() {
    if (auto res = sem_post(*this); res < 0)
      throw std::runtime_error("sem_wait");
  }
};


}
