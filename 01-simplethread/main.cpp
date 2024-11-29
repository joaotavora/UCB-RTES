#include <pthread.h>
#include <array>
#include <tuple>
#include <cstdio>
#include <vector>

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

template <typename F>
struct args_impl : args_impl<decltype(&F::operator())> {};

template <typename F, typename F2 = F>
struct thread_base {
  F f;
  auto getf() {return f;};
};

template <typename F, typename R, typename ...Args>
struct thread_base<F, R(*)(Args...)>  {
   auto getf() { return F{};}
};

int caramba();

const thread_base<decltype(&caramba)> mistery;

template <typename F, pthread_t Invalid = pthread_t{}>
struct thread : thread_base<F> {
  pthread_t t_{Invalid};
  args_impl<F>::type args_{};
public:
  thread() {}

  template <typename ...Args>
  thread(F f, Args... args) : thread_base<F>{f}, args_{std::make_tuple(args...)}{
    pthread_create(&t_, NULL, [](void* arg) -> void*{
      thread* self = reinterpret_cast<thread*>(arg); // NOLINT(*reinterpret-cast*)
      std::apply(self->getf(), self->args_); // SHADY
      return nullptr;
    }, this);
  }

  thread(const thread &) = delete;
  thread &operator=(const thread &) = delete;

  thread(thread && o) noexcept : thread_base<F>{o}, t_{std::move(o.t_)} {};

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

  

  using thread_t = xpto::thread<decltype(fn)>;
  static_assert(sizeof(thread_t)==sizeof(pthread_t) + sizeof(int));


  auto fn2 = [&, v=42](int tid) {
    printf("I'm just a stoopid little thread summing three things %d!\n", argc+v+tid);
  };
  std::vector<xpto::thread<decltype(fn2)>> vec;
  vec.emplace_back(fn2, 42);
  vec.emplace_back(fn2, 3);

  {
    std::array<thread_t, num_threads> threads;
  
    // threads.reserve(num_threads);
    for (auto i = 0u; i < num_threads; ++i) {
      threads.at(i) = xpto::thread{fn, i};
    }
  }
  // for (auto &t : threads) t.join();
  printf("Done!\n");
  return 0;
}
