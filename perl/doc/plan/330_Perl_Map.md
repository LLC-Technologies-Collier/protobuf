# Milestone 33: Perl Layer - Protobuf::Map

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/06-map.t`.
*   [x] Tests in `t/06-map.t` cover all methods of `Protobuf::Map` for various key and value types (scalar and message).
*   [ ] Add leak tests using Test::LeakTrace to `t/06-map.t`.
*   [ ] Add `ok_test_coverage('Protobuf::Map')` to `t/06-map.t`.
*   [x] Implement `Protobuf::Map` module (`lib/Protobuf/Map.pm`).
*   [x] Implement O(1) Bulk Map Projection.
*   [ ] TODO: Implement Shared-Arena Key Deduplication. (Difficulty: 6/10).
    *   [ ] TODO: Design key deduplication strategy for shared arenas. (Difficulty: 3/10).
    *   [ ] TODO: Implement internal hash table for tracking and reusing key strings in arenas. (Difficulty: 3/10).
    *   [ ] TODO: Verify memory savings and performance impact of key deduplication. (Difficulty: 3/10).
*   [ ] TODO: Provide Real-Time Map Collision Analysis API. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement O(1) Map-to-Hash Projection for zero-copy Perl access. (Difficulty: 6/10).
        *   [ ] TODO: Implement tie-based mechanism for Protobuf::Map to appear as a native Perl hash. (Difficulty: 3/10).
        *   [ ] TODO: Optimize internal accessors to minimize overhead during tied hash operations. (Difficulty: 3/10).
        *   [ ] TODO: Verify zero-copy behavior and benchmark against standard hash population. (Difficulty: 3/10).
    *   [ ] TODO: Implement SSE4.2-Accelerated Map Key Validation for bulk ingestion. (Difficulty: 8/10).
        *   [ ] TODO: Research SSE4.2 string processing instructions (e.g., PCMPESTRI). (Difficulty: 3/10).
        *   [ ] TODO: Implement vectorized validation logic for common key types (e.g., UTF-8 strings). (Difficulty: 3/10).
        *   [ ] TODO: Provide fallback implementation for non-SSE4.2 systems and verify correctness. (Difficulty: 3/10).
    *   [ ] TODO: Implement Self-Healing Map Structure Consistency background auditing. (Difficulty: 7/10).
        *   [ ] TODO: Implement background auditor for map internal structures. (Difficulty: 3/10).
        *   [ ] TODO: Implement logic to detect and repair bucket list corruption or cycle detection. (Difficulty: 3/10).
        *   [ ] TODO: Verify auditor stability and performance impact during active map mutations. (Difficulty: 3/10).
*   [x] Implement XS Bindings (`lib/Protobuf/Map.xs`).
*   [ ] Implement typemap for `upb_Map *` in `perl/typemap`.
*   [x] Perl tests pass for Protobuf::Map.
*   [x] Refine C API for Map (if any).
*   [x] C layer tests for Map still pass.
*   [x] All prior Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.