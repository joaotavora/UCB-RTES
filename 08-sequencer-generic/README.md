# Generic Sequencer

Upstream C sources:
[`sequencer_generic/`][1]

Generalizes the hardcoded sequencer: a 30 Hz sequencer thread releases
up to seven services at sub-rates (3 Hz, 1 Hz, 0.5 Hz, 0.1 Hz) by
modulo counting.  Includes `clock_times.c` for probing Linux clock resolutions
and the `raspbian-ccr/` kernel module enabling user-space access to
the ARM cycle counter.

The vendored `sequencer_generic/` here omits some files (`capture.c`,
`capturelib.c`, `seqv4l2.c`, `seqgen3.c`) and has comment-only edits;
top-level `clock_times.c` adds a `CLOCK_MONOTONIC_COARSE` loop.
`generic-sequencer.cpp` is the C++ rewrite (priority-queue release
list).

The C++ rewrite replaces modulo counting with a priority queue of
absolute deadlines, but is not yet feature-complete: sequencer and
services run with default (non-RT) attributes and no affinity, the
three services do no work (they only log their release), and the
seven-service set is reduced to three.  It shows the release-list
idea, not yet the RM-priority behavior the C programs demonstrate.

[1]: https://github.com/siewertsmooc/RTES-ECEE-5623/tree/e9f433560de63687d31196c5ef6e1604ff046a69/sequencer_generic
