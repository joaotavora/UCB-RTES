#pragma once

#include "xpto/c_resource.hpp"
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <format>
#include <stdexcept>
#include "xpto/orlose.hpp"

namespace xpto {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
struct sem : private xpto::c_resource<::sem_open, ::sem_close, SEM_FAILED> {
  using c_resource::c_resource;

  sem(const std::string& name = autoname(),
      int oflag = O_CREAT, mode_t mode = 0700, unsigned int value = 0)
  : c_resource{name.c_str(), oflag, mode, value}{}

  void wait() {
    xpto::or_lose(sem_wait(*this));
  }

  void post() {
    xpto::or_lose(sem_post(*this));
  }

  int value() {
    int x{};
    xpto::or_lose(sem_getvalue(*this, &x));
    return x;
  }

private:
  static constexpr std::string autoname() {
    static int counter{};
    return std::format("hmmm_{}", ++counter);
  }
};
#pragma GCC diagnostic pop


}
