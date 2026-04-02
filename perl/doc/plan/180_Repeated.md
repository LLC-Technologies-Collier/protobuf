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
    *   [ ] TODO: Implement O(1) Repeated-to-Array Projection for direct upb-to-Perl conversion. (Difficulty: 6/10).
    *   [ ] TODO: Implement SSE4.2-Accelerated Repeated Field Filtering utilities. (Difficulty: 8/10).
    *   [ ] TODO: Implement Self-Healing Repeated Structure Consistency background auditing. (Difficulty: 7/10).
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.