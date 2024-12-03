#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>

#include <print>
#include <stdexcept>

#include "xpto/c_resource.hpp"

namespace xpto {
struct sem : private xpto::c_resource<sem_t, ::sem_open, ::sem_close, SEM_FAILED> {
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


void wait1arg(int pid){::waitpid(pid, NULL, 0);}
struct child : xpto::c_resource<int, ::fork, wait1arg> {};

} // namespace xpto

int main() {
  try {
    std::println("Two procs");

    xpto::sem child_sem{"/childsem", O_CREAT, 0700, 0};
    xpto::sem parent_sem{"/parentsem", O_CREAT, 0700, 0};

    xpto::child child{};

    if (child.empty()) {
      for (auto i = 0; i < 3; ++i){
        std::println("Child: taking child_sem");
        child_sem.wait();
        std::println("Child: posting parent_sem");
        parent_sem.post();
      }
      std::println("Child: say bye bye!");
    } else {
      for (auto i = 0; i < 3; ++i){
        std::println("Parent: posting child_sem");
        child_sem.post();
        std::println("Parent: taking parent_sem");
        parent_sem.wait();
      }
      child.clear();
      std::println("Parent: say bye bye!");
    }
  } catch (std::exception e) {
    std::println(stderr, "Ooops {}", e.what());
  }

}
