# Milestone 37: Perl Layer - TextFormat

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/13-text_format.t`.
*   [x] Tests in `t/13-text_format.t` cover all functions in `Protobuf::TextFormat` for parsing and printing messages.
*   [ ] Add leak tests using Test::LeakTrace to `t/13-text_format.t`.
*   [ ] Add `ok_test_coverage('Protobuf::TextFormat')` to `t/13-text_format.t`.
*   [x] Implement Protobuf::TextFormat. (Implemented as `to_text` on `Protobuf::Message`)
*   [ ] TODO: Implement Direct-to-Stream Text Printing (C-Layer). (Difficulty: 6/10).
*   [x] Implement Lossless Text Formatting for Unknown Fields.
*   [ ] TODO: Provide Custom Text Format Redactors. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement VPP-Style SIMD Text Parsing for high-throughput ingestion. (Difficulty: 9/10).
    *   [ ] TODO: Implement Zero-Copy Text Projections from shared memory arenas. (Difficulty: 8/10).
    *   [ ] TODO: Implement Self-Healing Text Consistency background auditing. (Difficulty: 7/10).
*   [x] Perl tests pass.
*   [x] Refine C API (if any) and re-test.
*   [x] Review and update perl/doc/architecture/** documents.
