# Milestone 1: Build System & C Test Framework

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [x] Setup build system (MakeMaker) in `perl/Makefile.PL`.
*   [x] Configure `MY::postamble` for building `libprotobuf_common.$(DLEXT)`.
*   [x] Configure `MY::postamble` for compiling C tests in `perl/t/c/`.
*   [x] Implement C-level test harness (`perl/t/c/upb-perl-test.h`).
*   [ ] TODO: Implement configurable sanitizer support (ASan, UBSan, MSan) via environment variables.
*   [ ] TODO: Automate C-level benchmark compilation and execution (`make bench`).
*   [ ] TODO: Implement linker-level symbol visibility control for `libprotobuf_common.so` to ensure ABI stability.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
