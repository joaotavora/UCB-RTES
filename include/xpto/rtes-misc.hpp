#pragma once

#include <pthread.h>
#include <sched.h>
#include <unistd.h>

#include <cstdio>
#include "thread.hpp"

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

void install_rt_scheduler () {
  auto rt_max_prio = sched_get_priority_max(SCHED_FIFO);
  // Now let's set some stuff for the main process and its single thread
  sched_param main_param{};
  xpto::or_lose(sched_getparam(gettid(), &main_param));
  std::printf(
      "I'll now adjust the priority since it's %d right now (and that probably "
      "isn't acceptable for SCHED_FIFO)\n",
      main_param.sched_priority);
  main_param.sched_priority = rt_max_prio;
  xpto::or_lose(sched_setscheduler(gettid(), SCHED_FIFO, &main_param));
  std::printf("New RT scheduler installed on current thread\n");
  xpto::dump_scheduler();
}
}  // namespace xpto
