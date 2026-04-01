#pragma once

#include <pthread.h>
#include <sched.h>
#include <unistd.h>

#include <functional>
#include <memory>
#include <optional>
#include <set>
#include <string>

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
  struct thread_interface {
    thread_interface() = default;
    thread_interface(const thread_interface&) = delete;
    thread_interface(thread_interface&&) = default;
    thread_interface& operator=(const thread_interface&) = delete;
    thread_interface& operator=(thread_interface&&) = default;
    virtual ~thread_interface() = default;
  };

  // The trampoline runs with a pointer to this heap-allocated model,
  // which never moves: thread objects transfer the pointer, not the
  // storage, so a started thread can be moved safely.
  template <typename F>
  struct thread_model : thread_interface {
    pthread_t tid{};
    F f;

    thread_model(const thread_model&) = delete;
    thread_model(thread_model&&) = default;
    thread_model& operator=(const thread_model&) = delete;
    thread_model& operator=(thread_model&&) = default;

    thread_model(const attributes& attrs, F f) : f{std::move(f)} {
      // Create the thread with specified attributes,
      pthread_attr_t pattrs{};
      xpto::or_lose(pthread_attr_init(&pattrs));
      AUTO(pthread_attr_destroy(&pattrs));
 
      cpu_set_t cpuset;
      CPU_ZERO(&cpuset);
      if (attrs.affinity.size()) {
        for (auto x : attrs.affinity) CPU_SET(x, &cpuset);
        xpto::or_lose(
            pthread_attr_setaffinity_np(&pattrs, sizeof(cpuset), &cpuset));
      }
      if (attrs.stack_size) {
        xpto::or_lose(
            pthread_attr_setstacksize(&pattrs, attrs.stack_size.value()));
      }

      if (attrs.policy) {
        pthread_attr_setinheritsched(&pattrs, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&pattrs, attrs.policy.value());

        struct sched_param sparam{};
        int prio{};
        if (attrs.prio)
          sparam.sched_priority = attrs.prio.value();
        else
          sparam.sched_priority = sched_get_priority_max(attrs.policy.value());

        // Not sure if this one is needed
        xpto::or_lose(
            sched_setscheduler(getpid(), attrs.policy.value(), &sparam));

        pthread_attr_setschedparam(&pattrs, &sparam);
      }

      auto lambda = [](void* arg) -> void* {
        auto self = static_cast<thread_model*>(arg);
        std::invoke(self->f);
        return nullptr;
      };

      ZCALL_OR_LOSE(pthread_create(&tid, &pattrs, lambda, this));
      if (attrs.name.size()) {
        ZCALL_OR_LOSE(pthread_setname_np(tid, attrs.name.c_str()));
      }
    }

    ~thread_model() override {
      // FIXME: check joinable threads (errcode)
      // FIXME: make don't do this, add join method instead
      if (tid) pthread_join(tid, nullptr);
    }
  };

  std::unique_ptr<thread_interface> m_pimpl{};

 public:
  template <typename F>
    requires std::invocable<F>
  explicit thread(F&& f) : thread{attributes{}, std::forward<F>(f)} {}

  template <typename F>
    requires std::invocable<F>
  explicit thread(attributes a, F&& f)
      : m_pimpl{std::make_unique<thread_model<F>>(a, std::forward<F>(f))} {}

  thread() = default;
};

}  // namespace xpto
