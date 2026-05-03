# Milestone 29.B: Compatibility Tests Inspired by protobuf-perlxs

[TOC]

## Objective

To ensure our UPB-based Protobuf implementation covers key use cases and features demonstrated in the older `protobuf-perlxs` module, we will add integration tests based on its examples.

## Tasks

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

1.  **[ ] Adapt `string_bytes.proto` Test:**
    *   [ ] Copy `examples/string_bytes/string_bytes.proto` to `perl/t/protos/`.
    *   [ ] Create `perl/t/integration/299_1_string_bytes.t`.
    *   [ ] Test string and bytes fields with and without embedded NULLs.
    *   [ ] Test with multi-megabyte string/bytes values.
    *   [ ] Verify roundtrip fidelity.

2.  **[ ] Adapt `types.proto` Test:**
    *   [ ] Copy `examples/types/types.proto` to `perl/t/protos/`.
    *   [ ] Create `perl/t/integration/299_2_all_types.t`.
    *   [ ] Test all scalar types, enums, and nested messages.
    *   [ ] Verify constructor with HashRef.
    *   [ ] Verify `to_hashref` output.
    *   [ ] Test repeated fields of various types.
    *   [ ] Verify 64-bit integer handling (with Math::BigInt if necessary).

3.  **[ ] Adapt `error.proto` Test:**
    *   [ ] Copy `examples/error/error.proto` to `perl/t/protos/`.
    *   [ ] Create `perl/t/integration/299_3_required_fields.t`.
    *   [ ] Test that `serialize()` (or `pack`) croaks if required fields are missing.

4.  **[ ] Adapt `package.proto` Test:**
    *   [ ] Copy `examples/package/messages.proto` to `perl/t/protos/`.
    *   [ ] Create `perl/t/integration/299_4_packages.t`.
    *   [ ] Verify that generated modules are placed in the correct namespace corresponding to the proto package.

5.  **[ ] Adapt `embedded.proto` Test:**
    *   [ ] Copy `examples/embedded/error.proto` to `perl/t/protos/` (renaming to avoid conflict, e.g., `embedded_test.proto`).
    *   [ ] Create `perl/t/integration/299_5_embedded.t`.
    *   [ ] Focus tests on deep nesting and HashRef constructor.

6.  **[ ] Implement Missing Methods:**
    *   [ ] Add `fields()` method to `Protobuf::Message` (or generator).
    *   [ ] Implement `copy_from()` and `merge_from()` from HashRef in `Protobuf::Message` XS.
    *   [ ] Implement constructor from scalar binary in `Protobuf::Message` XS.
    *   [ ] Add tests for these methods in the relevant new `.t` files.

7.  **[ ] Documentation:**
    *   [ ] Update relevant architecture documents if any new design decisions are made.
    *   [ ] Mark this document as complete.
