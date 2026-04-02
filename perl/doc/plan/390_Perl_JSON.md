# Milestone 39: Perl Layer - JSON

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/14-json_format.t`.
*   [x] Tests in `t/14-json_format.t` cover all functions in `Protobuf::JSON` for parsing and printing messages, including WKTs.
*   [ ] Add leak tests using Test::LeakTrace to `t/14-json_format.t`.
*   [ ] Add `ok_test_coverage('Protobuf::JSON')` to `t/14-json_format.t`.
*   [x] Implement Protobuf::JSON. (Implemented directly on `Protobuf::Message` as `to_json` and `from_json`)
*   [ ] TODO: Implement Direct JSON-to-Wire Conversion.
*   [x] Implement Streaming JSON Serialization. (Skeletal implementation).
*   [ ] TODO: Verify JSON Schema Mapping consistency.
*   [x] Perl tests pass.
*   [x] Refine C API (if any) and re-test.
*   [x] Review and update perl/doc/architecture/** documents.
