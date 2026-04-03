# Milestone 39: Perl Layer - JSON

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/14-json_format.t`.
*   [x] Tests in `t/14-json_format.t` cover all functions in `Protobuf::JSON` for parsing and printing messages, including WKTs.
*   [ ] Add leak tests using Test::LeakTrace to `t/14-json_format.t`.
*   [ ] Add `ok_test_coverage('Protobuf::JSON')` to `t/14-json_format.t`.
*   [x] Implement Protobuf::JSON. (Implemented directly on `Protobuf::Message` as `to_json` and `from_json`)
*   [ ] Add direct wire-format encoder to JSON bridge. (Difficulty: 3/10).
*   [ ] Verify roundtrip consistency for binary JSON fields. (Difficulty: 2/10).
*   [ ] Add schema-mapping validation tests. (Difficulty: 2/10).
*   [ ] Verify JSON-to-Proto field name mapping consistency. (Difficulty: 3/10).

*   **Engineering Excellence (Reach for More):**
    *   [ ] Implement VPP-style dispatch for JSON tokenization. (Difficulty: 3/10).
    *   [ ] Add SSE4.2/AVX2 kernels for string-to-number conversion. (Difficulty: 3/10).
    *   [ ] Implement mmap attachment for JSON input buffers. (Difficulty: 3/10).
    *   [ ] Add metadata for zero-copy JSON projection. (Difficulty: 3/10).
    *   [ ] Implement audit_json() consistency checks. (Difficulty: 3/10).
    *   [ ] Add background audit of reified JSON objects. (Difficulty: 2/10).

*   [x] Perl tests pass.
*   [x] Refine C API (if any) and re-test.
*   [x] Review and update perl/doc/architecture/** documents.
