# Milestone 35: Perl Layer - Well-Known Types

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **Protobuf::Any:**
    *   [x] Create test file `t/07-any.t`.
    *   [x] Tests in `t/07-any.t` cover all methods of `Protobuf::Any`, including packing and unpacking.
    *   [ ] TODO: Implement Direct Any Unpacking (C-Layer).
    *   [ ] Add leak tests using Test::LeakTrace to `t/07-any.t`.
    *   [ ] Add `ok_test_coverage('Protobuf::Any')` to `t/07-any.t`.
    *   [x] Implement Perl and XS layers for Protobuf::Any.
    *   [x] Perl tests pass for Protobuf::Any.
    *   [x] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::Duration:**
    *   [x] Create test file `t/08-duration.t`.
    *   [x] Tests in `t/08-duration.t` cover all methods of `Protobuf::Duration`.
    *   [ ] TODO: Implement Vectorized Duration Conversions.
    *   [ ] Add leak tests using Test::LeakTrace to `t/08-duration.t`.
    *   [ ] Add `ok_test_coverage('Protobuf::Duration')` to `t/08-duration.t`.
    *   [x] Implement Perl and XS layers for Protobuf::Duration.
    *   [x] Perl tests pass for Protobuf::Duration.
    *   [x] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::Timestamp:**
    *   [x] Create test file `t/09-timestamp.t`.
    *   [x] Tests in `t/09-timestamp.t` cover all methods of `Protobuf::Timestamp`.
    *   [ ] TODO: Implement Vectorized Timestamp Conversions.
    *   [ ] Add leak tests using Test::LeakTrace to `t/09-timestamp.t`.
    *   [ ] Add `ok_test_coverage('Protobuf::Timestamp')` to `t/09-timestamp.t`.
    *   [x] Implement Perl and XS layers for Protobuf::Timestamp.
    *   [x] Perl tests pass for Protobuf::Timestamp.
    *   [x] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::FieldMask:**
    *   [x] Create test file `t/10-fieldmask.t`.
    *   [x] Tests in `t/10-fieldmask.t` cover all methods of `Protobuf::FieldMask`.
    *   [ ] Add leak tests using Test::LeakTrace to `t/10-fieldmask.t`.
    *   [ ] Add `ok_test_coverage('Protobuf::FieldMask')` to `t/10-fieldmask.t`.
    *   [x] Implement Perl and XS layers for Protobuf::FieldMask.
    *   [x] Perl tests pass for Protobuf::FieldMask.
    *   [x] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::Struct:**
    *   [x] Create test file `t/11-struct.t`.
    *   [x] Tests in `t/11-struct.t` cover all methods of `Protobuf::Struct`, `Protobuf::Value`, and `Protobuf::ListValue`.
    *   [x] Implement Struct-Specific Memory Profiling (Skeletal implementation).
    *   [ ] Add leak tests using Test::LeakTrace to `t/11-struct.t`.
    *   [ ] Add `ok_test_coverage()` for `Protobuf::Struct`, `Protobuf::Value`, `Protobuf::ListValue` to `t/11-struct.t`.
    *   [x] Implement Perl and XS layers for Struct, Value, ListValue.
    *   [x] Perl tests pass for Struct, Value, ListValue.
    *   [x] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::Wrappers:**
    *   [x] Create test file `t/12-wrappers.t`.
    *   [x] Tests in `t/12-wrappers.t` cover all methods of all `Protobuf::*Value` wrapper types.
    *   [ ] Add leak tests using Test::LeakTrace to `t/12-wrappers.t`.
    *   [ ] Add `ok_test_coverage()` for all `Protobuf::*Value` wrappers to `t/12-wrappers.t`.
    *   [x] Implement Perl and XS layers for Wrapper types. (Handled natively by auto-generation, no special methods needed)
    *   [x] Perl tests pass for Wrappers.
    *   [x] Refine C API (if any) and re-test C & Perl.

*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
