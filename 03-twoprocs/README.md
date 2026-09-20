# Two Synchronized Processes

Upstream C source:
[`twoprocs/twoprocs.c`][1]

`fork()`s a child and synchronizes parent and child in a ping-pong
loop using named POSIX semaphores (`sem_open`/`sem_wait`/`sem_post`) —
inter-process synchronization instead of threads.

`main.c` is byte-identical to the upstream file.

`main.cpp` is functionally identical — the same 3-round ping-pong —
but built from `xpto` RAII wrappers: `xpto::sem` (a `c_resource` over
`sem_open`/`sem_close`) and a local `xpto::forked_child` (over
`fork`/`waitpid`).  Unlike the C version, it never `sem_unlink`s the
named semaphores.

[1]: https://github.com/siewertsmooc/RTES-ECEE-5623/blob/e9f433560de63687d31196c5ef6e1604ff046a69/twoprocs/twoprocs.c
