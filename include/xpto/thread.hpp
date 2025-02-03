#pragma once

#include <pthread.h>
#include <sched.h>

#include <cstdio>
#include <memory>
#include <optional>
#include <set>
#include <tuple>

#include "auto.hpp"
#include "util.hpp"

namespace xpto {

void dump_scheduler(std::FILE* stream = stdout) {
  int schedType{}, scope{};
  pthread_attr_t main_attr;

  schedType = sched_getscheduler(getpid());

  switch (schedType) {
    case SCHED_FIFO:
      fprintf(stream, "Pthread Policy is SCHED_FIFO\n");
      break;
    case SCHED_OTHER:
      fprintf(stream, "Pthread Policy is SCHED_OTHER\n");
      break;
    case SCHED_RR:
      fprintf(stream, "Pthread Policy is SCHED_RR\n");
      break;
    default:
      fprintf(stream, "Pthread Policy is UNKNOWN\n");
  }

  pthread_attr_getscope(&main_attr, &scope);

  if (scope == PTHREAD_SCOPE_SYSTEM)
    fprintf(stream, "PTHREAD SCOPE SYSTEM\n");
  else if (scope == PTHREAD_SCOPE_PROCESS)
    fprintf(stream, "PTHREAD SCOPE PROCESS\n");
  else
    fprintf(stream, "PTHREAD SCOPE UNKNOWN\n");
}

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

   private:
  };

 private:
  struct thread_interface {
    thread_interface() = default;
    thread_interface(const thread_interface &) = delete;
    thread_interface(thread_interface &&) = default;
    thread_interface &operator=(const thread_interface &) = delete;
    thread_interface &operator=(thread_interface &&) = default;
    virtual ~thread_interface() = default;
  };

  template <typename F, typename... Args>
  struct thread_model : thread_interface {
    pthread_t tid_{};
    F f_;
    std::tuple<Args...> args_;
    thread_model(const thread_model &) = delete;
    thread_model(thread_model &&) = default;
    thread_model &operator=(const thread_model &) = delete;
    thread_model &operator=(thread_model &&) = default;

    thread_model(const attributes &attrs, F f, Args... args)
        : f_{std::move(f)}, args_{std::move(args)...} {
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

      auto lambda = [](void *arg) -> void * {
        auto self = static_cast<thread_model *>(arg);
        std::apply(self->f_, self->args_);
        return nullptr;
      };

      ZCALL_OR_LOSE(pthread_create(&tid_, &pattrs, lambda, this));
      if (attrs.name.size()) {
        ZCALL_OR_LOSE(pthread_setname_np(tid_, attrs.name.c_str()));
      }
    }
    ~thread_model() override { pthread_join(tid_, nullptr); }
  };

  std::unique_ptr<thread_interface> pimpl_;

 public:
  template <typename F, class... Args>
    requires std::invocable<F, Args...>
  explicit thread(F &&f, Args &&...args)
      : pimpl_{std::make_unique<thread_model<F, Args...>>(
            attributes{}, std::forward<F>(f), std::forward<Args>(args)...)} {}

  template <typename F, class... Args>
    requires std::invocable<F, Args...>
  explicit thread(const attributes &attr, F &&f, Args &&...args)
      : pimpl_{std::make_unique<thread_model<F, Args...>>(
            attr, std::forward<F>(f), std::forward<Args>(args)...)} {}

  thread() = default;
};

}  // namespace xpto
