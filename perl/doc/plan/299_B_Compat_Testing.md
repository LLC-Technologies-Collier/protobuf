# Milestone 29.B: Compatibility Tests Inspired by protobuf-perlxs

[TOC]

## Objective

To ensure our UPB-based Protobuf implementation covers key use cases and features demonstrated in the older `protobuf-perlxs` module, we will add integration tests based on its examples.

## Tasks

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

1.  **[x] Adapt `string_bytes.proto` Test:**
    *   [x] Copy `examples/string_bytes/string_bytes.proto` to `perl/t/protos/`.
    *   [x] Create `perl/t/integration/299_1_string_bytes.t`.
    *   [x] Test string and bytes fields with and without embedded NULLs.
    *   [x] Test with multi-megabyte string/bytes values.
    *   [x] Verify roundtrip fidelity.

2.  **[x] Adapt `types.proto` Test:**
    *   [x] Copy `examples/types/types.proto` to `perl/t/protos/`.
    *   [x] Create `perl/t/integration/299_2_all_types.t`. (Renamed to `299_2_types.t`)
    *   [x] Test all scalar types, enums, and nested messages.
    *   [x] Verify constructor with HashRef.
    *   [x] Verify `to_hashref` output.
    *   [x] Test repeated fields of various types.
    *   [x] Verify 64-bit integer handling (Verified in `t/432_bigint_roundtrip.t`).

3.  **[x] Adapt `error.proto` Test:**
    *   [x] Copy `examples/error/error.proto` to `perl/t/protos/`.
    *   [x] Create `perl/t/integration/299_3_required_fields.t`. (Renamed to `299_3_error.t`)
    *   [x] Test that `serialize()` (or `pack`) croaks if required fields are missing.

4.  **[x] Adapt `package.proto` Test:**
    *   [x] Copy `examples/package/messages.proto` to `perl/t/protos/`. (Renamed to `messages.proto`)
    *   [x] Create `perl/t/integration/299_4_packages.t`. (Renamed to `299_4_package.t`)
    *   [x] Verify that generated modules are placed in the correct namespace corresponding to the proto package.

5.  **[x] Adapt `embedded.proto` Test:**
    *   [x] Copy `examples/embedded/error.proto` to `perl/t/protos/` (Renamed to `embedded_error.proto`).
    *   [x] Create `perl/t/integration/299_5_embedded.t`.
    *   [x] Focus tests on deep nesting and HashRef constructor.

6.  **[ ] Implement Missing Methods:**
    *   [ ] Add `fields()` method to `Protobuf::Message` (or generator).
    *   [ ] Implement `copy_from()` and `merge_from()` from HashRef in `Protobuf::Message` XS.
    *   [ ] Implement constructor from scalar binary in `Protobuf::Message` XS. (Partially covered by `parse()`)
    *   [ ] Add tests for these methods in the relevant new `.t` files.

7.  **[ ] Documentation:**
    *   [ ] Update relevant architecture documents if any new design decisions are made.
    *   [ ] Mark this document as complete.
