# Milestone 37: Perl Layer - TextFormat

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/13-text_format.t`.
*   [x] Tests in `t/13-text_format.t` cover all functions in `Protobuf::TextFormat` for parsing and printing messages.
*   [ ] Add leak tests using Test::LeakTrace to `t/13-text_format.t`.
*   [ ] Add `ok_test_coverage('Protobuf::TextFormat')` to `t/13-text_format.t`.
*   [x] Implement Protobuf::TextFormat. (Implemented as `to_text` on `Protobuf::Message`)
*   [ ] TODO: XS logic to call upb_text_encode and print directly to Perl's STDOUT or a file handle. (Difficulty: 3/10).
*   [ ] TODO: Benchmark direct stream printing vs intermediate string representation. (Difficulty: 2/10).
*   [ ] TODO: Handle large message sizes without excessive memory allocation. (Difficulty: 2/10).
*   [x] Implement Lossless Text Formatting for Unknown Fields.
*   [ ] TODO: Provide Custom Text Format Redactors. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Research existing SIMD-accelerated text parsers for Protobuf. (Difficulty: 3/10).
    *   [ ] TODO: Prototype SIMD-optimized parser for basic Protobuf field types. (Difficulty: 3/10).
    *   [ ] TODO: Implement VPP-style batching for text ingestion. (Difficulty: 3/10).
    *   [ ] TODO: Compare SIMD parser performance with the standard upb parser. (Difficulty: 3/10).
    *   [ ] TODO: Design mechanism to return read-only string views from arena-allocated text. (Difficulty: 3/10).
    *   [ ] TODO: Implement XS logic for creating Perl strings from these views without copying. (Difficulty: 3/10).
    *   [ ] TODO: Ensure memory safety and lifetime management for zero-copy strings. (Difficulty: 3/10).
    *   [ ] TODO: Test zero-copy projections under concurrent access. (Difficulty: 2/10).
    *   [ ] TODO: Logic to verify that parsed text matches the original binary representation. (Difficulty: 3/10).
    *   [ ] TODO: Background thread (Coro) to periodically audit text consistency. (Difficulty: 3/10).
    *   [ ] TODO: Implement error reporting and potential automatic repair mechanism. (Difficulty: 3/10).
*   [x] Perl tests pass.
*   [x] Refine C API (if any) and re-test.
*   [x] Review and update perl/doc/architecture/** documents.
