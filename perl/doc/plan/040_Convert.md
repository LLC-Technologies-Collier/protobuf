# Milestone 4: C Layer - convert

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

## C Test File Structure

*   C tests for type conversions are located in `perl/t/c/convert/`.
*   Helper functions and common test setup are in `perl/t/c/convert/test_util.c`.
*   Tests for individual protobuf types are in separate files within `perl/t/c/convert/types/`, e.g., `perl/t/c/convert/types/int32.c`.
*   Main test runners `perl/t/c/convert/upb_to_sv.c` and `perl/t/c/convert/sv_to_upb.c` will include and execute tests from the `types/` directory.
*   `perl/t/c/c_test_config.json` is updated to build these runners.

## Tasks

1.  **Implement `perl/xs/convert/upb_to_sv.c`:**
    *   [x] Implement functions to convert from UPB to Perl for all types.

2.  **Implement `perl/xs/convert/sv_to_upb.c`:**
    *   [x] Implement functions to convert from Perl to UPB for all types.

3.  **Implement and Test Each Type:**

    For each type below, create/update the `<type>.c` file in `perl/t/c/convert/types/`, add test cases to the main runners, and ensure tests pass *for that type* before moving to the next.

    *   [x] **Int32**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **UInt32**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Int64**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **UInt64**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Float**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Double**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Bool**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **String**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Bytes**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Enum**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Fixed32**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Fixed64**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **SFixed32**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **SFixed64**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **SInt32**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Int64**
        *   [ ] Implement & Test UPB -> SV (TODO: Handle BigInt).
        *   [x] Implement & Test SV -> UPB
    *   [x] **UInt64**
        *   [ ] Implement & Test UPB -> SV (TODO: Handle BigInt).
        *   [x] Implement & Test SV -> UPB
    ...
    *   [x] **Group**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB

    4.  **Reach for More (World-Class Goals):**
    *   [ ] TODO: Implement transparent Math::BigInt support for 64-bit integer overflows.
    *   [ ] TODO: Implement zero-copy ByteBuffer conversions for large String/Bytes fields.
    *   [ ] TODO: Implement range-validated strict type checking for narrowing conversions.
    *   [ ] TODO: Implement **Type-Fuzzing:** Verify that conversion logic fails safely (croaks) when passed incompatible Perl SV types (e.g., CodeRefs, Globs) without memory corruption.
    *   [ ] TODO: Implement **Boundary-Fuzzing:** Verify that integer conversions correctly handle boundary conditions (MIN/MAX) and overflow/underflow situations with consistent error reporting.

    5.  **Final Review:**
    *   [x] Ensure all C tests in `perl/t/c/convert/` pass.

5.  **Documentation:**
    *   [x] Review and update perl/doc/architecture/** documents based on this milestone's implementation.
