# Assignment 1 — Hello World (Simple Thread Creation)

Upstream C source:
[`simplethread/pthread.c`][1]

The first graded assignment: the actual assignment version in
`pthread.cpp` logs "Hello World from Main!/Thread!" to syslog with
course/assignment tags.

`pthread.cpp` also runs `uname -a` (via `popen`, guarded by
`xpto::or_lose` and the `AUTO` scope guard), tags each syslog entry
`[COURSE:1][ASSIGNMENT:1]`, and spawns the thread with `xpto::thread`,
whose destructor joins it before main exits.

[1]: https://github.com/siewertsmooc/RTES-ECEE-5623/blob/e9f433560de63687d31196c5ef6e1604ff046a69/simplethread/pthread.c
