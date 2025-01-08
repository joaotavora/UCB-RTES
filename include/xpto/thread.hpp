#pragma once

#include <pthread.h>
#include <sched.h>

#include <memory>
#include <set>
#include <tuple>

#include "util.hpp"

namespace xpto {

class thread {
 public:
  struct attributes {
    std::set<int> affinity;
    size_t stack_size = 0;
    std::string name = {};
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
    thread_model(const attributes &attr, F f, Args... args)
        : f_{std::move(f)}, args_{std::move(args)...} {
      cpu_set_t cpuset;
      // Initialize the CPU set
      CPU_ZERO(&cpuset);
      for (auto x : attr.affinity) CPU_SET(x, &cpuset);

      // Create the thread with specified affinity
      pthread_attr_t pattr;
      CFUNCALL(pthread_attr_init(&pattr));
      CFUNCALL(pthread_attr_setaffinity_np(&pattr, sizeof(cpu_set_t), &cpuset));
      CFUNCALL(pthread_attr_setstacksize(&pattr, attr.stack_size));

      CFUNCALL(pthread_create(
          &tid_, nullptr,
          [](void *arg) -> void * {
            auto self = static_cast<thread_model *>(arg);
            std::apply(self->f_, self->args_);
            return nullptr;
          },
          this));
    }
    ~thread_model() override { pthread_join(tid_, nullptr); }
  };

  std::unique_ptr<thread_interface> pimpl_;

 public:
  template <typename F, class... Args>
    requires std::invocable<F, Args...>
  explicit thread(F &&f, Args &&...args)
      : thread{attributes{}, std::forward<F>(f), std::forward<Args>(args)...} {}

  template <typename F, class... Args>
    requires std::invocable<F, Args...>
  explicit thread(const attributes &attr, F &&f, Args &&...args)
      : pimpl_{std::make_unique<thread_model<F, Args...>>(
            attr, std::forward<F>(f), std::forward<Args>(args)...)} {}

  thread() = default;
};

}  // namespace xpto
