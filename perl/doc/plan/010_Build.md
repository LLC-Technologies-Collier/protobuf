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
*   [x] Implement XS helper to wrap existing messages within attached arenas (reification). (Difficulty: 3/10).
*   [x] Implement cross-process message handle serialization (path+offset). (Difficulty: 2/10).
*   [x] Implement SELinux security context verification for shared blocks. (Difficulty: 2/10).
*   [x] Create verified IPC handshake integration test (`t/413_tmpfs_ipc_attach.t`). (Difficulty: 3/10).
*   [x] Implement Batch Validation API for collecting field vectors. (Difficulty: 2/10).
*   [x] Implement SSE4.1 SIMD kernel for narrow integer range-checking. (Difficulty: 3/10).
*   [x] Implement AVX2 SIMD kernel for bulk string validation. (Difficulty: 3/10).
*   [x] Implement dynamic VPP dispatcher based on CPUID. (Difficulty: 3/10).

*   **Engineering Excellence (Reach for More):**
    *   **Guard Page Support:**
        *   [x] Design guard page strategy for BlockAlloc. (Difficulty: 3/10)
        *   [x] Implement mmap/mprotect logic for guard pages. (Difficulty: 3/10)
        *   [ ] Add tests for guard page functionality. (Difficulty: 2/10)
    *   **Fuzzing:**
        *   [ ] Integrate libFuzzer for wire format parsing. (Difficulty: 5/10)
        *   [ ] Add CI infrastructure for daily fuzzing runs. (Difficulty: 4/10)
    *   **Cross-Platform CI:**
        *   [x] Research GitHub Actions for macOS builds. (Difficulty: 2/10)
        *   [ ] Research GitHub Actions for Windows builds (Strawberry Perl). (Difficulty: 3/10) // Deferred pending local CI setup
        *   [ ] Implement basic macOS CI workflow. (Difficulty: 3/10)
        *   [ ] Implement basic Windows CI workflow. (Difficulty: 3/10)
