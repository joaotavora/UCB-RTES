#include <pthread.h>
#include <array>
#include <tuple>
#include <cstdio>

static constexpr size_t num_threads{64};
namespace xpto {

template<typename FF>
struct args_impl;

template <typename C, typename R, typename ...Args>
struct args_impl<R(C::*)(Args...)> {
  using type = std::tuple<Args...>;
};

template <typename C, typename R, typename ...Args>
struct args_impl<R(C::*)(Args...) const> {
  using type = std::tuple<Args...>;
};

// template <typename R, typename ...Args>
// struct args_impl<R(*)(Args...)> {
//   using type = std::tuple<Args...>;
// };

template <typename F>
struct args_impl : args_impl<decltype(&F::operator())> {};


template <auto F, pthread_t Invalid = pthread_t{}>
struct thread {
  pthread_t t_{Invalid};
  args_impl<decltype(F)>::type args_{};
public:
  thread() {}

  template <typename ...Args>
  thread(Args... args) : args_{std::make_tuple(args...)}{
    pthread_create(&t_, NULL, [](void* arg) -> void*{
      thread* self = reinterpret_cast<thread*>(arg); // NOLINT(*reinterpret-cast*)
      std::apply(F, self->args_);
      return nullptr;
    }, this);
  }

  thread(const thread &) = delete;
  thread &operator=(const thread &) = delete;

  thread(thread && o) noexcept {
    std::swap(o.t_, this->t_);
  };
  thread &operator=(thread && rhs) noexcept {
    std::swap(t_, rhs.t_);
    return *this;
  }
  void join() {
    pthread_join(t_, NULL);
    t_ = Invalid;
  }

  ~thread() {
    if ( t_!= Invalid) join();
  }
};
}

int main(int argc, char *argv[]) {

  auto fn = [](int tid) {
    size_t sum{};
    for (int i = 1; i <= tid; ++i) sum += i;
    printf("Thread[%d] sum[1..%d]=%lu\n", tid, tid, sum);
  };

  {
    std::array<xpto::thread<fn>, num_threads> threads;
  
    // threads.reserve(num_threads);
    for (auto i = 0u; i < num_threads; ++i) {
      threads.at(i) = xpto::thread<fn>{i};
    }
  }
  // for (auto &t : threads) t.join();
  printf("Done!\n");
  return 0;
}
