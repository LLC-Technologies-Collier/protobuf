# Milestone 4: C Layer - convert

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

## C Test File Structure

*   C tests for type conversions are located in `t/c/convert/`.
*   Helper functions and common test setup are in `t/c/convert/test_util.c`.
*   Tests for individual protobuf types are in separate files within `t/c/convert/types/`, e.g., `types/int32.c`, `types/string.c`.
*   Main test runners `t/c/convert/upb_to_sv.c` and `t/c/convert/sv_to_upb.c` will include and execute tests from the `types/` directory.
*   `t/c/c_test_config.json` is updated to build these runners with all necessary `.c` files from `types/` and `test_util.c` as `extra_src`.

## Tasks

1.  **Implement `xs/convert/upb_to_sv.c`:**
    *   [ ] Implement functions to convert from UPB to Perl for all types.

2.  **Implement `xs/convert/sv_to_upb.c`:**
    *   [ ] Implement functions to convert from Perl to UPB for all types.

3.  **Implement and Test Each Type:**

    For each type below, create/update the `<type>.c` file in `t/c/convert/types/`, add test cases to the main runners, and ensure tests pass *for that type* before moving to the next.

    *   [ ] **Int32**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **UInt32**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **Int64**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **UInt64**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **Float**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **Double**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **Bool**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **String**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **Bytes**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **Enum**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **Fixed32**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **Fixed64**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **SFixed32**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **SFixed64**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **SInt32**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **SInt64**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **Message**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB
    *   [ ] **Group**
        *   [ ] Implement & Test UPB -> SV
        *   [ ] Implement & Test SV -> UPB

4.  **Final Review:**
    *   [ ] Ensure all C tests in `t/c/convert/` pass.

5.  **Documentation:**
    *   [ ] Review and update perl/doc/architecture/** documents based on this milestone's implementation.
