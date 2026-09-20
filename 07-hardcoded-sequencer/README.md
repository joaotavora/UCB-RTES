# Hardcoded Sequencer

Upstream C source:
[`sequencer/lab1.c`][1]

A top-priority sequencer thread releases two periodic services, S1
(T=20 ms, C=10 ms) and S2 (T=50 ms, C=20 ms), over semaphores at a
hardcoded schedule — a cyclic-executive-style release pattern at 90%
utilization, demonstrating RM preemption and the critical instant.

`lab1.c` matches upstream except for `void main` → `int main` and an
include reordering.

`main.cpp` keeps the same hardcoded schedule, but: `xpto::work_for()`
calibrates `FIB_TEST` iterations so each service burns exactly its C
(10/20 ms) instead of hand-tuning, `xpto::thread`/`xpto::sem` replace
the raw pthread calls, all threads are pinned to core 3 (the C file's
`setaffinity` line is commented out), and an `std::atomic` abort flag
with final semaphore posts replaces the C `abortTest` ints.

[1]: https://github.com/siewertsmooc/RTES-ECEE-5623/blob/e9f433560de63687d31196c5ef6e1604ff046a69/sequencer/lab1.c
