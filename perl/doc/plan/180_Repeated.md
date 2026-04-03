# Milestone 18: C Layer - repeated

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/repeated/composite.c:**
    *   [x] Create test file `perl/t/c/repeated/composite.c`. (TODO stub created).
    *   [ ] Tests cover all public functions in `xs/repeated/composite.h`.
    *   [x] Implement C functions.
    *   [ ] C tests pass.
*   **perl/xs/repeated/repeated.c:**
    *   [x] Create test file `perl/t/c/repeated/repeated.c`.
    *   [x] Tests cover all public functions in `xs/repeated/repeated.h`. (Verified wrapper creation and size).
    *   [ ] TODO: Implement SIMD-accelerated scalar appending.
    *   [ ] TODO: Implement COW (Copy-On-Write) for large repeated fields.
    *   [ ] TODO: Implement In-Place Sort/Binary Search utilities.
    *   [x] Implement C functions.
    *   [x] C tests pass.
*   **perl/xs/repeated/scalar.c:**
    *   [ ] Create test file `perl/t/c/repeated/scalar.c`. (TODO stub created).
    *   [ ] Tests cover all public functions in `xs/repeated/scalar.h`.
    *   [ ] Implement C functions.
    *   [ ] C tests pass.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement O(1) Repeated-to-Array Projection for direct upb-to-Perl conversion:
        *   [ ] TODO: Implement XS-level `AV` (Perl Array) wrapping for `upb_Array`. (Difficulty: 3/10)
        *   [ ] TODO: Create magic-based `FETCH`/`STORE` for projected Perl arrays. (Difficulty: 3/10)
        *   [ ] TODO: Implement direct pointer mapping for scalar repeated fields. (Difficulty: 2/10)
    *   [ ] TODO: Implement SSE4.2-Accelerated Repeated Field Filtering utilities:
        *   [ ] TODO: Implement SSE4.2 string/integer comparison kernels for `grep`-like filtering. (Difficulty: 3/10)
        *   [ ] TODO: Create vectorized "find" utility for `upb_Array`. (Difficulty: 3/10)
        *   [ ] TODO: Implement dispatcher for CPU-specific optimization paths. (Difficulty: 2/10)
    *   [ ] TODO: Implement Self-Healing Repeated Structure Consistency background auditing:
        *   [ ] TODO: Implement background auditor for `upb_Array` capacity vs. size consistency. (Difficulty: 3/10)
        *   [ ] TODO: Create sub-message identity validator for repeated message fields. (Difficulty: 2/10)
        *   [ ] TODO: Implement automated repair for "ghost" entries in repeated fields. (Difficulty: 2/10)
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.