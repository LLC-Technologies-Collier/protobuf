# Milestone 31: Perl Layer - Protobuf::RepeatedField

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/05-repeated.t`.
*   [x] Tests in `t/05-repeated.t` cover all methods of `Protobuf::RepeatedField` for various scalar and message types.
*   [ ] Add leak tests using Test::LeakTrace to `t/05-repeated.t`.
*   [ ] Add `ok_test_coverage('Protobuf::RepeatedField')` to `t/05-repeated.t`.
*   [x] Implement `Protobuf::RepeatedField` module (`lib/Protobuf/RepeatedField.pm`). (Implemented as Protobuf::Internal::Repeated)
*   [ ] TODO: Implement SIMD-Accelerated Bulk Append for Perl arrays.
*   [ ] TODO: Implement Native XS grep/map for tied arrays.
*   [ ] TODO: Provide Lazy-Wrapper Deferral for high-throughput scanning.
*   [x] Implement XS Bindings (`lib/Protobuf/RepeatedField.xs`).
*   [ ] Implement typemap for `upb_Array *` in `perl/typemap`.
*   [x] Perl tests pass for Protobuf::RepeatedField.
*   [x] Refine C API for RepeatedField (if any).
*   [x] C layer tests for RepeatedField still pass.
*   [x] All prior Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.