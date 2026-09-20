# Simple Thread Affinity

Upstream C source:
[`simplethread-affinity/pthread.c`][1]

Inspects the main thread's CPU affinity mask with
`pthread_getaffinity_np()` and then spawns 64 `SCHED_FIFO` threads
from a starter thread, reporting which CPU each one runs on with
`sched_getcpu()` — showing how threads are placed on cores.

`main.c` matches upstream except for an added `#include <unistd.h>`.

`main.cpp` drops the affinity-mask inspection and instead pins the
starter and all 64 workers to core 3 via `xpto::thread` attributes
(`.affinity = {3}`, `SCHED_FIFO`); it times with `std::chrono` rather
than `gettimeofday()`.

[1]: https://github.com/siewertsmooc/RTES-ECEE-5623/blob/e9f433560de63687d31196c5ef6e1604ff046a69/simplethread-affinity/pthread.c
