# Milestone 1: Build System & C Test Framework

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [x] Setup build system (MakeMaker) in `perl/Makefile.PL`.
*   [x] Configure `MY::postamble` for building `libprotobufperl.$(DLEXT)`.
*   [x] Configure `MY::postamble` for compiling C tests in `perl/t/c/`.
*   [x] Implement C-level test harness (`perl/t/c/upb-perl-test.h`).
*   [x] Implement configurable sanitizer support (ASan, UBSan, MSan) via environment variables.
*   [x] Automate C-level benchmark compilation and execution (`make bench`).
*   [x] Implement linker-level symbol visibility control for `libprotobufperl.so` to ensure ABI stability (via `libprotobufperl.map`).
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
*   [x] Implement a global audit trail and trace-level debugging for arena and object lifecycles. (Implemented in `obj_cache.c`).
*   [x] Implement C-layer infrastructure for tmpfs-backed arenas (mmap/ftruncate). (Implemented in `arena_tmpfs.c`).
*   [ ] Implement XS helper to wrap existing messages within attached arenas (reification). (Difficulty: 3/10).
*   [ ] Implement cross-process message handle serialization (path+offset). (Difficulty: 2/10).
*   [ ] Implement SELinux security context verification for shared blocks. (Difficulty: 2/10).
*   [ ] Create verified IPC handshake integration test (`t/412_tmpfs_ipc.t` expansion). (Difficulty: 3/10).
*   [ ] Implement Batch Validation API for collecting field vectors. (Difficulty: 2/10).
*   [ ] Implement SSE4.1 SIMD kernel for narrow integer range-checking. (Difficulty: 3/10).
*   [ ] Implement AVX2 SIMD kernel for bulk string validation. (Difficulty: 3/10).
*   [ ] Implement dynamic VPP dispatcher based on CPUID. (Difficulty: 3/10).
