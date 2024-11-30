#include <pthread.h>
#include <tuple>

namespace xpto {

template <typename F, typename = void>
struct thread_base;

template <typename F>
struct thread_base<F, std::enable_if_t<std::is_empty_v<F>>> {
  thread_base() = default;
  thread_base(F) {}
  template <typename... Args>
  void invoke(Args&&... args) {
    F{}(std::forward<Args>(args)...);
  }
};

template <typename F>
struct thread_base<
    F, std::enable_if_t<!(std::is_pointer_v<F> || std::is_empty_v<F>)>> {
  F f;
  template <typename... Args>
  void invoke(Args&&... args) {
    f(std::forward<Args>(args)...);
  }
};

template <typename F, pthread_t Invalid = pthread_t{}>
class thread : thread_base<F> {
  template <typename FF>
  struct arg_extractor;

  template <typename C, typename R, typename... Args>
  struct arg_extractor<R (C::*)(Args...)> {
    using type = std::tuple<std::decay_t<Args>...>;
  };

  template <typename C, typename R, typename... Args>
  struct arg_extractor<R (C::*)(Args...) const> {
    using type = std::tuple<std::decay_t<Args>...>;
  };

  template <typename FF>
  struct arg_extractor : arg_extractor<decltype(&FF::operator())> {};

  arg_extractor<F>::type args_{};
  pthread_t t_{Invalid};

 public:
  thread() {}

  template <typename... Args>
  thread(F f, Args&&... args)
      : thread_base<F>{f},
        args_{std::forward_as_tuple(std::forward<Args>(args)...)} {
    pthread_create(
        &t_, NULL,
        [](void* arg) -> void* {
          thread* self =
              reinterpret_cast<thread*>(arg);  // NOLINT(*reinterpret-cast*)
          std::apply(
              [&](auto&&... args) {
                self->invoke(std::forward<decltype(args)>(args)...);
              },
              self->args_);
          return nullptr;
        },
        this);
  }

  thread(const thread&) = delete;
  thread& operator=(const thread&) = delete;

  thread(thread&& o) noexcept : thread_base<F>{o}, t_{std::move(o.t_)} {};

  thread& operator=(thread&& rhs) noexcept {
    std::swap(t_, rhs.t_);
    return *this;
  }
  void join() {
    pthread_join(t_, NULL);
    t_ = Invalid;
  }

  ~thread() {
    if (t_ != Invalid) join();
  }
};
}
