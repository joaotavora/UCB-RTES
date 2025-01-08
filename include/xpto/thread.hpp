#include <pthread.h>
#include <tuple>

#include <memory>

namespace xpto {

class thread {
public:
  struct attributes {};

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
    thread_model(const attributes& attr, F f, Args... args)
        : f_{std::move(f)}, args_{std::move(args)...} {
      pthread_create(&tid_, nullptr, [](void* arg) -> void*{
        auto self = static_cast<thread_model*>(arg);
        std::apply(self->f_, self->args_);
        return nullptr;
      }, this);
    }
    ~thread_model() override {
      pthread_join(tid_, nullptr);
    }
  };

  std::unique_ptr<thread_interface> pimpl_;
public:
  template <typename F, class... Args>
  explicit thread(F&& f, Args&&... args)
  : thread{attributes{}, std::forward(f), std::forward<Args>(args)...}  {}

  template <typename F, class... Args>
  explicit thread(const attributes& attr, F&& f, Args&&... args)
      : pimpl_{std::make_unique<thread_model<F, Args...>>(
          std::forward<F>(f), std::forward<Args>(args)...)} {}

  thread() = default;
};

}  // namespace xpto
