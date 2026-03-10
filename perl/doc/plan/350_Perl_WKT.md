# Milestone 35: Perl Layer - Well-Known Types

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **Protobuf::Any:**
    *   [ ] Create test file `t/07-any.t`.
    *   [ ] Tests in `t/07-any.t` cover all methods of `Protobuf::Any`, including packing and unpacking.
    *   [ ] Add leak tests using Test::LeakTrace to `t/07-any.t`.
    *   [ ] Add `ok_test_coverage('Protobuf::Any')` to `t/07-any.t`.
    *   [ ] Implement Perl and XS layers for Protobuf::Any.
    *   [ ] Perl tests pass for Protobuf::Any.
    *   [ ] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::Duration:**
    *   [ ] Create test file `t/08-duration.t`.
    *   [ ] Tests in `t/08-duration.t` cover all methods of `Protobuf::Duration`.
    *   [ ] Add leak tests using Test::LeakTrace to `t/08-duration.t`.
    *   [ ] Add `ok_test_coverage('Protobuf::Duration')` to `t/08-duration.t`.
    *   [ ] Implement Perl and XS layers for Protobuf::Duration.
    *   [ ] Perl tests pass for Protobuf::Duration.
    *   [ ] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::Timestamp:**
    *   [ ] Create test file `t/09-timestamp.t`.
    *   [ ] Tests in `t/09-timestamp.t` cover all methods of `Protobuf::Timestamp`.
    *   [ ] Add leak tests using Test::LeakTrace to `t/09-timestamp.t`.
    *   [ ] Add `ok_test_coverage('Protobuf::Timestamp')` to `t/09-timestamp.t`.
    *   [ ] Implement Perl and XS layers for Protobuf::Timestamp.
    *   [ ] Perl tests pass for Protobuf::Timestamp.
    *   [ ] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::FieldMask:**
    *   [ ] Create test file `t/10-fieldmask.t`.
    *   [ ] Tests in `t/10-fieldmask.t` cover all methods of `Protobuf::FieldMask`.
    *   [ ] Add leak tests using Test::LeakTrace to `t/10-fieldmask.t`.
    *   [ ] Add `ok_test_coverage('Protobuf::FieldMask')` to `t/10-fieldmask.t`.
    *   [ ] Implement Perl and XS layers for Protobuf::FieldMask.
    *   [ ] Perl tests pass for Protobuf::FieldMask.
    *   [ ] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::Struct:**
    *   [ ] Create test file `t/11-struct.t`.
    *   [ ] Tests in `t/11-struct.t` cover all methods of `Protobuf::Struct`, `Protobuf::Value`, and `Protobuf::ListValue`.
    *   [ ] Add leak tests using Test::LeakTrace to `t/11-struct.t`.
    *   [ ] Add `ok_test_coverage()` for `Protobuf::Struct`, `Protobuf::Value`, `Protobuf::ListValue` to `t/11-struct.t`.
    *   [ ] Implement Perl and XS layers for Struct, Value, ListValue.
    *   [ ] Perl tests pass for Struct, Value, ListValue.
    *   [ ] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::Wrappers:**
    *   [ ] Create test file `t/12-wrappers.t`.
    *   [ ] Tests in `t/12-wrappers.t` cover all methods of all `Protobuf::*Value` wrapper types.
    *   [ ] Add leak tests using Test::LeakTrace to `t/12-wrappers.t`.
    *   [ ] Add `ok_test_coverage()` for all `Protobuf::*Value` wrappers to `t/12-wrappers.t`.
    *   [ ] Implement Perl and XS layers for Wrapper types.
    *   [ ] Perl tests pass for Wrappers.
    *   [ ] Refine C API (if any) and re-test C & Perl.

*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
