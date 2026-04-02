# Milestone 8: C Layer - descriptor_containers

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/descriptor_containers/by_name_map.c:**
    *   [x] Create test file `perl/t/c/descriptor_containers/by_name_map.c`.
    *   [x] Tests cover all public functions in the corresponding .h file, including checks for re-entrancy and safe state management.
    *   [x] TODO: Implement Direct-to-Hash Projection for high-frequency bulk access.
    *   [ ] TODO: Verify concurrent iterator stability during interleaved read-only access.
    *   [x] Implement C functions.
    *   [x] C tests pass.
*   **perl/xs/descriptor_containers/by_number_map.c:**
    *   [x] Create test file `perl/t/c/descriptor_containers/by_number_map.c`.
    *   [x] Tests cover all public functions in the corresponding .h file, including checks for re-entrancy and safe state management.
    *   [ ] TODO: Implement O(1) reverse lookup (Value-to-Key) optimization.
    *   [x] Implement C functions.
    *   [x] C tests pass.
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
