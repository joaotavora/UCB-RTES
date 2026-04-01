#pragma once

#include <pthread.h>
#include <sched.h>

#include <functional>
#include <memory>
#include <optional>
#include <set>
#include <tuple>

#include "xpto/auto.hpp"
#include "xpto/orlose.hpp"

namespace xpto {

class thread {
 public:
  struct attributes {
    enum policy_e : char {
      FIFO = SCHED_FIFO,
      RR = SCHED_RR,
      OTHER = SCHED_OTHER
    };

    std::set<int> affinity{};
    std::optional<size_t> stack_size{};
    std::string name{};
    std::optional<policy_e> policy{};
    std::optional<int> prio{};
  };

 private:
  std::move_only_function<void()> f_;
  pthread_t tid_{};
  attributes attrs_{};

 public:
  template <typename F>
    requires std::invocable<F>
  explicit thread(F&& f) : thread{{}, std::forward<F>(f)} {}

  friend void swap(thread& a, thread& b) noexcept {
    std::swap(a.f_, b.f_);
    std::swap(a.tid_, b.tid_);
    std::swap(a.attrs_, b.attrs_);
  }

  thread() = default;
  thread(const thread&) = delete;
  thread& operator=(const thread&) = delete;
  thread(thread&& o) noexcept
      : f_{std::move(o.f_)}, tid_{o.tid_}, attrs_{std::move(o.attrs_)} {
    o.tid_ = 0;
  }
  thread& operator=(thread&& rhs) noexcept {
    if (&rhs != this) {
      thread tmp{std::move(rhs)};
      swap(*this, tmp);
    }
    return *this;
  }

  template <typename F>
    requires std::invocable<F>
  explicit thread(attributes a, F&& f)
      : attrs_{std::move(a)}, f_{std::forward<F>(f)} {
    // Create the thread with specified attributes,
    pthread_attr_t pattrs{};
    xpto::or_lose(pthread_attr_init(&pattrs));
    AUTO(pthread_attr_destroy(&pattrs));

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    if (attrs_.affinity.size()) {
      for (auto x : attrs_.affinity) CPU_SET(x, &cpuset);
      xpto::or_lose(
          pthread_attr_setaffinity_np(&pattrs, sizeof(cpuset), &cpuset));
    }
    if (attrs_.stack_size) {
      xpto::or_lose(
          pthread_attr_setstacksize(&pattrs, attrs_.stack_size.value()));
    }

    if (attrs_.policy) {
      pthread_attr_setinheritsched(&pattrs, PTHREAD_EXPLICIT_SCHED);
      pthread_attr_setschedpolicy(&pattrs, attrs_.policy.value());

      struct sched_param sparam{};
      int prio{};
      if (attrs_.prio)
        sparam.sched_priority = attrs_.prio.value();
      else
        sparam.sched_priority = sched_get_priority_max(attrs_.policy.value());

      // Not sure if this one is needed
      xpto::or_lose(
          sched_setscheduler(getpid(), attrs_.policy.value(), &sparam));

      pthread_attr_setschedparam(&pattrs, &sparam);
    }

    auto lambda = [](void* arg) -> void* {
      auto self = static_cast<thread*>(arg);
      std::invoke(self->f_);
      return nullptr;
    };

    ZCALL_OR_LOSE(pthread_create(&tid_, &pattrs, lambda, this));
    if (attrs_.name.size()) {
      ZCALL_OR_LOSE(pthread_setname_np(tid_, attrs_.name.c_str()));
    }
  }

  ~thread() {
    // FIXME: check joinable threads (errcode)
    // FIXME: make don't do this, add join method instead
    if (tid_) pthread_join(tid_, nullptr);
  }
};

}  // namespace xpto
