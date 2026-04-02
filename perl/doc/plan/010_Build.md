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
*   [ ] TODO: Implement VPP-style vector processing for bulk field validation using SSE4.1/AVX2. (Difficulty: 8/10).
*   [ ] TODO: Implement zero-copy IPC transport using tmpfs-backed shared memory arenas. (Difficulty: 7/10).
*   [ ] TODO: Implement a global audit trail and trace-level debugging for arena and object lifecycles. (Difficulty: 5/10).
