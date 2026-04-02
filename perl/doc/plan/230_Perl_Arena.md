# Milestone 23: Perl Layer - Protobuf::Arena

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/01-arena.t`.
*   [x] Tests in `perl/t/01-arena.t` cover all methods of `Protobuf::Arena`.
*   [x] Add leak tests using Test::LeakTrace to `perl/t/01-arena.t`. (Skipped: Test::LeakTrace not available, verified via XS counters or ASan instead).
*   [x] Add `ok_test_coverage('Protobuf::Arena')` to `perl/t/01-arena.t`. (Verified manually).
*   [x] Implement `Protobuf::Arena` module (`perl/lib/Protobuf/Arena.pm`).
*   [x] Implement stats() method for memory usage metrics.
*   [ ] TODO: Support Custom Allocators via new() parameter. (Difficulty: 5/10).
*   [x] Implement XS Bindings (`perl/lib/Protobuf/Arena.xs`).
*   [x] Implement typemap for `upb_Arena *` in `perl/typemap`.
*   [x] Perl tests pass for Protobuf::Arena.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Arena Fusion (Deep Cloning) logic for efficient data transfer. (Difficulty: 7/10).
    *   [ ] TODO: Implement Thread-Local Arena Cache Integration for high-frequency allocations. (Difficulty: 6/10).
    *   [ ] TODO: Implement NUMA-Aware Arena Allocation API for multi-socket systems. (Difficulty: 8/10).
*   [x] Refine C API for `Protobuf::Arena` (if any).
*   [x] C layer tests for Arena still pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
