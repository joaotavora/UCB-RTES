#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fmt/core.h>

#include "xpto/c_resource.hpp"
#include "xpto/semaphore.hpp"

namespace xpto {
void wait1arg(int pid){::waitpid(pid, NULL, 0);}
struct forked_child : xpto::c_resource<::fork, wait1arg, 0> {
  forked_child() : c_resource{noargs_construct} {}
};


} // namespace xpto

int main() {
  try {
    fmt::println("Two procs");

    xpto::sem child_sem{"/childsem", O_CREAT, 0700, 0};
    xpto::sem parent_sem{"/parentsem", O_CREAT, 0700, 0};

    xpto::forked_child child{};

    if (child.empty()) {
      for (auto i = 0; i < 3; ++i){
        fmt::println("Child: taking child_sem");
        child_sem.wait();
        fmt::println("Child: posting parent_sem");
        parent_sem.post();
      }
      fmt::println("Child: say bye bye!");
    } else {
      for (auto i = 0; i < 3; ++i){
        fmt::println("Parent: posting child_sem");
        child_sem.post();
        fmt::println("Parent: taking parent_sem");
        parent_sem.wait();
      }
      child.clear();
      fmt::println("Parent: say bye bye!");
    }
  } catch (std::exception e) {
    fmt::println(stderr, "Ooops {}", e.what());
  }

}
