# Milestone 32: Integration - Perl RepeatedField

[TOC]

*   [x] Create test file `t/integration/320_repeated_field.t`
*   [ ] TODO: Implement Direct Array-to-Array Deep Copy.
*   [x] Implement Shared-Arena Array Slicing. (Skeletal implementation).
*   [ ] TODO: Verify Cross-Interpreter Container Migration Stress.
*   [x] Tests cover interactions between Protobuf::RepeatedField and Protobuf::Message.
*   [x] Integration tests pass.
*   [x] Create and implement `xt/coro/320_repeated_field.t` to test Coro safety.
*   [ ] TODO: Stress concurrent repeated field mutation (100 Coros).
*   [ ] TODO: Verify Coro re-entrancy during array subset slicing.
*   [x] Create and implement `xt/mojo/320_repeated_field.t` to test Mojo::IOLoop concurrency safety.
*   [ ] Create and implement `xt/anyevent/320_repeated_field.t` to test AnyEvent concurrency safety. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Script to spawn 10k Coros and perform repeated field mutations. (Difficulty: 3/10).
    *   [ ] TODO: Monitor memory usage and leaks during massive concurrency stress. (Difficulty: 2/10).
    *   [ ] TODO: Implement back-pressure or resource limiting if necessary. (Difficulty: 3/10).
    *   [ ] TODO: Verify data integrity after massive concurrent mutations. (Difficulty: 2/10).
    *   [ ] TODO: Research upb and SSE intrinsics for array aggregation. (Difficulty: 3/10).
    *   [ ] TODO: Implement XS hooks for SSE-accelerated Sum/Min/Max operations. (Difficulty: 3/10).
    *   [ ] TODO: Fallback implementation for non-SSE platforms. (Difficulty: 2/10).
    *   [ ] TODO: Benchmark SSE vs non-SSE aggregation performance. (Difficulty: 2/10).
    *   [ ] TODO: Design checksum or parity-based consistency check for repeated fields. (Difficulty: 3/10).
    *   [ ] TODO: Implement background audit thread (using Coro) for array consistency. (Difficulty: 3/10).
    *   [ ] TODO: Logic to report or repair detected inconsistencies. (Difficulty: 3/10).
    *   [ ] TODO: Test audit performance overhead on main thread. (Difficulty: 2/10).
*   [x] Review and update perl/doc/architecture/** documents.