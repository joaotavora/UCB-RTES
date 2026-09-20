# Simple Thread Creation

Upstream C source:
[`simplethread/pthread.c`][1]

The "hello world" of POSIX threads: `N` threads are created with
`pthread_create()`, each sums `1..tid` locally, prints, and is reaped
with `pthread_join()`.

Note: `main.c` here is a rewrite of the same exercise (same semantics,
but 64 threads instead of 12 and renamed structs), not a verbatim
copy; `main.cpp` is a C++ port of the rewrite.

[1]: https://github.com/siewertsmooc/RTES-ECEE-5623/blob/e9f433560de63687d31196c5ef6e1604ff046a69/simplethread/pthread.c
