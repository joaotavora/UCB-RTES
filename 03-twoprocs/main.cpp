#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>

#include <print>

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
    std::println("Two procs");

    xpto::sem child_sem{"/childsem", O_CREAT, 0700, 0};
    xpto::sem parent_sem{"/parentsem", O_CREAT, 0700, 0};

    xpto::forked_child child{};

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
