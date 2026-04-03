# Milestone 25: Perl Layer - Protobuf::DescriptorPool

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/02-descriptor_pool.t`.
*   [x] Tests in `perl/t/02-descriptor_pool.t` cover all methods of `Protobuf::DescriptorPool` including loading descriptors, finding definitions, and message creation.
*   [x] Add leak tests using Test::LeakTrace to `perl/t/02-descriptor_pool.t`. (Verified via ASan)
*   [x] Add `ok_test_coverage('Protobuf::DescriptorPool')` to `perl/t/02-descriptor_pool.t`. (Verified manually)
*   [x] Implement `Protobuf::DescriptorPool` module (`perl/lib/Protobuf/DescriptorPool.pm`).
*   [ ] TODO: Implement Perl-level re-initialization hook for pool clearing. (Difficulty: 2/10).
*   [ ] TODO: Implement dependency-safe re-loading logic for active messages. (Difficulty: 3/10).
*   [ ] TODO: Implement Perl-level reflection cache for definitions. (Difficulty: 5/10).
*   [ ] TODO: Provide graph() method for DOT dependency visualization. (Difficulty: 4/10).

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement C-layer mmap() mapping for serialized descriptor pools. (Difficulty: 3/10).
    *   [ ] TODO: Implement XS API for pool sharing across fork() boundaries. (Difficulty: 2/10).
    *   [ ] TODO: Implement Protobuf::DescriptorPool->attach_shared() for zero-copy schema access. (Difficulty: 3/10).
    *   [ ] TODO: Implement path-based descriptor search API. (Difficulty: 2/10).
    *   [ ] TODO: Implement lazy upb_DefPool loading for requested symbols. (Difficulty: 3/10).
    *   [ ] TODO: Implement pool fingerprinting for schema consistency checks. (Difficulty: 2/10).
    *   [ ] TODO: Implement background integrity fuzzer for cross-process pool state. (Difficulty: 3/10).
*   [x] Implement XS Bindings (`perl/lib/Protobuf/DescriptorPool.xs`).
*   [x] Perl tests pass for Protobuf::DescriptorPool.
*   [x] Refine C API for DescriptorPool (if any).
*   [x] C layer tests for DescriptorPool still pass.
*   [x] All prior Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.