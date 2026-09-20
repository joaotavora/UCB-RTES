# Real-Time Simple Threading

Upstream C source:
[`rt_simple_threading/pthread.c`][1]

Four `SCHED_FIFO` threads at descending RT priorities (RM-style) run a
timed `FIB_TEST` compute section; main first raises itself to top RT
priority. Demonstrates RT scheduling with `sched_setscheduler()`,
priority assignment, and measuring execution time (`C`) with
`clock_gettime()`.

`pthread.c` is semantically identical to upstream — reformatted, with
the Jetson `sysctl` comment block removed.

`pthread.cpp` is the same demo with `xpto` conveniences:
`xpto::install_rt_scheduler()`/`xpto::dump_scheduler()` replace the
hand-rolled scheduler setup, `xpto::thread` attributes carry policy
and priority, `std::chrono` durations replace the `delta_t()` helper,
and `FIB_TEST` is retuned (seq 250 vs 47).

[1]: https://github.com/siewertsmooc/RTES-ECEE-5623/blob/e9f433560de63687d31196c5ef6e1604ff046a69/rt_simple_threading/pthread.c
