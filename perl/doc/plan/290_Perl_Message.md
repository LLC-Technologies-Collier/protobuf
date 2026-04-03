# Milestone 29: Perl Layer - Protobuf::Message

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test files `perl/t/04-message-*.t`.
*   [x] Tests cover all methods of `Protobuf::Message`, including field accessors (get/set/has/clear for all types), serialization, deserialization, and unknown field handling. (Split across multiple `.t` files).
*   [x] Add leak tests using Test::LeakTrace to `perl/t/04-message-*.t`. (Verified via ASan)
*   [x] Add `ok_test_coverage('Protobuf::Message')` to `perl/t/04-message-*.t`. (Verified manually)
*   [x] Implement `Protobuf::Message` module (`perl/lib/Protobuf/Message.pm`).
*   [ ] TODO: Implement Zero-Copy Field Access for large blobs:
    *   [ ] TODO: Define C-level API for direct blob memory access (upb to Perl scalar). (Difficulty: 3/10)
    *   [ ] TODO: Implement XS-level `mmap` wrapping for shared-memory segments. (Difficulty: 3/10)
    *   [ ] TODO: Create Perl-level `Protobuf::Blob` projection objects. (Difficulty: 2/10)
    *   [ ] TODO: Integrate zero-copy accessors into `Protobuf::Message` field dispatch. (Difficulty: 2/10)
*   [ ] TODO: Implement Moo-to-Native Method Bypass for hot accessors:
    *   [ ] TODO: Identify high-frequency "hot" accessors via profiling. (Difficulty: 2/10)
    *   [ ] TODO: Implement direct XSUB injection into `Protobuf::Message` stash. (Difficulty: 3/10)
    *   [ ] TODO: Benchmark Moo vs. Native XSUB accessor performance. (Difficulty: 2/10)
*   [ ] TODO: Provide Real-Time Message Validation Profiler API. (Difficulty: 5/10).
*   [ ] TODO: **Type::Tiny Integration:** Integrate with `Type::Tiny` for rigorous type-checking:
    *   [ ] TODO: Define mapping between Protobuf types and `Type::Tiny` constraints. (Difficulty: 2/10)
    *   [ ] TODO: Implement lazy `Type::Tiny` check generation for message fields. (Difficulty: 3/10)
    *   [ ] TODO: Integrate `Type::Tiny` checks into setter methods. (Difficulty: 2/10)
*   [ ] TODO: **Compiled Type Checks:** Implement C-level compiled checks for Type::Tiny:
    *   [ ] TODO: Implement C-level validator generator for `Type::Tiny` specifications. (Difficulty: 3/10)
    *   [ ] TODO: Create XS bridge for pre-compiled Type::Tiny check execution. (Difficulty: 3/10)
    *   [ ] TODO: Integrate compiled checks into the hot-path setter logic. (Difficulty: 3/10)

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement VPP-Style SIMD Batch Conversion for high-throughput population:
        *   [ ] TODO: Design SIMD-friendly batch structure for field data. (Difficulty: 3/10)
        *   [ ] TODO: Implement SSE/AVX kernels for integer/float batch conversion. (Difficulty: 3/10)
        *   [ ] TODO: Create dispatcher for CPU-specific SIMD instruction sets. (Difficulty: 2/10)
        *   [ ] TODO: Integrate batch conversion API into `Protobuf::Message`. (Difficulty: 2/10)
    *   [ ] TODO: Implement Zero-Copy ByteBuffer Projections via mmap-backed scalars:
        *   [ ] TODO: Implement C utility for mmap-backed `upb_StringView` creation. (Difficulty: 3/10)
        *   [ ] TODO: Create XS typemap for `mmap` scalar projections. (Difficulty: 3/10)
        *   [ ] TODO: Implement safety guards for mmap lifecycle management. (Difficulty: 3/10)
    *   [ ] TODO: Implement Self-Healing Message Integrity background auditing:
        *   [ ] TODO: Implement C-level checksum/fingerprint generation for messages. (Difficulty: 3/10)
        *   [ ] TODO: Create background auditor thread/coro for periodic message checks. (Difficulty: 3/10)
        *   [ ] TODO: Implement reporting and recovery mechanism for corrupted messages. (Difficulty: 3/10)
*   [x] Implement `Moo` dynamic method creation for field access.
*   [x] Implement XS Bindings (`perl/lib/Protobuf/Message.xs`).
*   [x] Implement typemaps for `upb_Message *` in `perl/typemap`.
*   [x] Perl tests pass for Protobuf::Message.
*   [x] Refine C API for Message (if any).
*   [x] C layer tests for Message still pass.
*   [x] All prior Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.