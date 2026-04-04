# Milestone 8: C Layer - descriptor_containers

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/descriptor_containers/by_name_map.c:**
    *   [x] Create test file `perl/t/c/descriptor_containers/by_name_map.c`.
    *   [x] Tests cover all public functions in the corresponding .h file, including checks for re-entrancy and safe state management.
    *   [ ] Implement Direct-to-Hash Projection for high-frequency bulk access. (Difficulty: 4/10)
    *   [ ] Add libcoro stress for interleaved container iteration. (Difficulty: 3/10).
    *   [ ] Implement read-only locks for container iteration. (Difficulty: 2/10).
    *   [x] Implement C functions.
    *   [x] C tests pass.
*   **perl/xs/descriptor_containers/by_number_map.c:**
    *   [x] Create test file `perl/t/c/descriptor_containers/by_number_map.c`.
    *   [x] Tests cover all public functions in the corresponding .h file, including checks for re-entrancy and safe state management.
    *   [ ] Add reverse-mapping metadata to NumberMap. (Difficulty: 2/10).
    *   [ ] Implement Value-to-Key lookup using linear search fallback. (Difficulty: 3/10).
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **Engineering Excellence (Reach for More):**
    *   [ ] Implement lazy blessing for descriptor wrappers. (Difficulty: 3/10).
    *   [ ] Add thread-local cache for common descriptor wrappers. (Difficulty: 3/10).
    *   [ ] Implement hash-table for mapping enum values to names. (Difficulty: 3/10).
    *   [ ] Add batch retrieval method to DescriptorContainer XS. (Difficulty: 3/10).
    *   [ ] Implement vectorized descriptor name matching. (Difficulty: 3/10).
    *   [ ] **Compact Containers:** Use arrays for small containers, upgrade to hash. (Difficulty: 7/10)
    *   [ ] **Iterator State Cache:** Cache iterator states. (Difficulty: 6/10)
    *   [ ] **Thread-Safe Epochs:** Epoch-based GC for lock-free reads. (Difficulty: 9/10)
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
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
