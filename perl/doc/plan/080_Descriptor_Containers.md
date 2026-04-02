# Milestone 8: C Layer - descriptor_containers

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/descriptor_containers/by_name_map.c:**
    *   [x] Create test file `perl/t/c/descriptor_containers/by_name_map.c`.
    *   [x] Tests cover all public functions in the corresponding .h file, including checks for re-entrancy and safe state management.
    *   [x] Implement Direct-to-Hash Projection for high-frequency bulk access.
    *   [ ] TODO: Verify concurrent iterator stability during interleaved read-only access. (Difficulty: 5/10).
    *   [x] Implement C functions.
    *   [x] C tests pass.
*   **perl/xs/descriptor_containers/by_number_map.c:**
    *   [x] Create test file `perl/t/c/descriptor_containers/by_number_map.c`.
    *   [x] Tests cover all public functions in the corresponding .h file, including checks for re-entrancy and safe state management.
    *   [ ] TODO: Implement O(1) reverse lookup (Value-to-Key) optimization. (Difficulty: 6/10).
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Lazy Wrapper Blessing with Thread-Local Cache to reuse descriptor wrappers. (Difficulty: 7/10).
    *   [ ] TODO: Implement O(1) Reverse-Index for Enum Value-to-Name lookups. (Difficulty: 6/10).
    *   [ ] TODO: Implement Vectorized Batch-Lookup API for bulk descriptor retrieval. (Difficulty: 8/10).
*   **perl/xs/descriptor_containers/generic_sequence.c:**
    *   [x] Create test file `perl/t/c/descriptor_containers/generic_sequence.c`.
    *   [x] Tests cover all public functions in the corresponding .h file, including checks for re-entrancy and safe state management.
    *   [x] Implement C functions.
    *   [x] C tests pass.
*   **perl/xs/descriptor_containers/iterators.c:**
    *   [x] Create test file `perl/t/c/descriptor_containers/iterators.c`.
    *   [x] Tests cover all public functions in the corresponding .h file, including checks for re-entrancy and safe state management.
    *   [x] Implement C functions.
    *   [x] C tests pass.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
