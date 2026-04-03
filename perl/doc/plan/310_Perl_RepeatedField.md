# Milestone 31: Perl Layer - Protobuf::RepeatedField

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/05-repeated.t`.
*   [x] Tests in `t/05-repeated.t` cover all methods of `Protobuf::RepeatedField` for various scalar and message types.
*   [ ] Add leak tests using Test::LeakTrace to `t/05-repeated.t`.
*   [ ] Add `ok_test_coverage('Protobuf::RepeatedField')` to `t/05-repeated.t`.
*   [x] Implement `Protobuf::RepeatedField` module (`lib/Protobuf/RepeatedField.pm`). (Implemented as Protobuf::Internal::Repeated)
*   [ ] Add bulk-append method to RepeatedField XS. (Difficulty: 3/10).
*   [ ] Implement AVX2 kernel for copying Perl scalars to UPB arrays. (Difficulty: 3/10).
*   [ ] Implement deferred wrapper inflation in FETCH. (Difficulty: 3/10).
*   [ ] Add fast-path scanning that bypasses SV creation. (Difficulty: 3/10).

*   **Engineering Excellence (Reach for More):**
    *   [ ] Implement array-to-AV projection for specific scalar types. (Difficulty: 3/10).
    *   [ ] Add metadata for zero-copy array sharing. (Difficulty: 3/10).
    *   [ ] Implement SSE4.2 find() for numeric arrays. (Difficulty: 3/10).
    *   [ ] Add vectorized grep() for fixed-length strings. (Difficulty: 3/10).
    *   [ ] Implement audit_consistency() for repeated fields. (Difficulty: 3/10).
    *   [x] Add background audit of array bounds and canaries.

*   [x] Implement XS Bindings (`lib/Protobuf/RepeatedField.xs`).
*   [ ] Implement typemap for `upb_Array *` in `perl/typemap`.
*   [x] Perl tests pass for Protobuf::RepeatedField.
*   [x] Refine C API for RepeatedField (if any).
*   [x] C layer tests for RepeatedField still pass.
*   [x] All prior Perl tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.