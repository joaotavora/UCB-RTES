# Increment / Decrement Threads

Upstream C source:
[`incdecthread/pthread.c`][1]

Demonstrates the unsynchronized read-modify-write data race: one
thread increments a shared global while another decrements it, and the
final value is (usually) wrong.

`pthread.c` is the upstream demo verbatim.  `main.cpp` ports it to C++
keeping the racy `gsum` and contrasting with `std::atomic`, using
`xpto::thread` instead of `std::thread`.

[1]: https://github.com/siewertsmooc/RTES-ECEE-5623/blob/e9f433560de63687d31196c5ef6e1604ff046a69/incdecthread/pthread.c
