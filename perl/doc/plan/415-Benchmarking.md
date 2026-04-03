# Milestone 41.5: Benchmarking

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [ ] REFRESH: Review @perl/doc/architecture/050-Benchmarking.md
*   [x] Implement benchmark scripts in `xt/benchmark/`.
*   [x] Run benchmarks for key operations (serialization, deserialization).
*   [ ] Compare results with other Perl Protobuf modules. (Difficulty: 5/10).
*   [ ] Document benchmark findings in doc/benchmarks.md. (Difficulty: 4/10).

*   **Engineering Excellence (Reach for More):**
    *   [ ] Add VPP-style batching to the message benchmark runner. (Difficulty: 3/10).
    *   [ ] Compare scalar vs. vectorized field processing throughput. (Difficulty: 3/10).
    *   [ ] Measure latency of tmpfs arena attachment vs. malloc. (Difficulty: 3/10).
    *   [ ] Bench cross-process message passing throughput via shared memory. (Difficulty: 3/10).
    *   [ ] Implement baseline performance tracking in a JSON schema. (Difficulty: 2/10).
    *   [ ] Add a post-build step to compare current stats vs. baseline. (Difficulty: 3/10).
