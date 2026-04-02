# Milestone 39: Perl Layer - JSON

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/14-json_format.t`.
*   [x] Tests in `t/14-json_format.t` cover all functions in `Protobuf::JSON` for parsing and printing messages, including WKTs.
*   [ ] Add leak tests using Test::LeakTrace to `t/14-json_format.t`.
*   [ ] Add `ok_test_coverage('Protobuf::JSON')` to `t/14-json_format.t`.
*   [x] Implement Protobuf::JSON. (Implemented directly on `Protobuf::Message` as `to_json` and `from_json`)
*   [ ] TODO: Implement Direct JSON-to-Wire Conversion. (Difficulty: 7/10).
*   [x] Implement Streaming JSON Serialization.
*   [ ] TODO: Verify JSON Schema Mapping consistency. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement VPP-Style SIMD JSON Parsing for high-throughput ingestion. (Difficulty: 9/10).
    *   [ ] TODO: Implement Zero-Copy JSON Projections from shared memory arenas. (Difficulty: 8/10).
    *   [ ] TODO: Implement Self-Healing JSON Consistency background auditing. (Difficulty: 7/10).
*   [x] Perl tests pass.
*   [x] Refine C API (if any) and re-test.
*   [x] Review and update perl/doc/architecture/** documents.
