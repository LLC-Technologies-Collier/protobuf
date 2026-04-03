# Milestone 20: C Layer - unknown_fields

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/unknown_fields/build.c:**
    *   [x] Create test file `perl/t/c/unknown_fields/build.c`. (TODO stub created).
    *   [ ] Tests cover all public functions in `xs/unknown_fields/build.h`.
    *   [ ] Implement C functions.
    *   [ ] C tests pass.
*   **perl/xs/unknown_fields/set.c:**
    *   [x] Create test file `perl/t/c/unknown_fields/set.c`.
    *   [x] Tests cover all public functions in `xs/unknown_fields/set.h`. (Verified wrapper creation and initial state).
    *   [ ] Add tag-to-mdef lookup infrastructure. (Difficulty: 3/10).
    *   [ ] Implement conversion from raw Unknown buffer to C struct. (Difficulty: 3/10).

    *   [ ] Implement hash-table for O(1) tag lookup. (Difficulty: 3/10).
    *   [ ] Add circular ring buffer for Unknown field events. (Difficulty: 2/10).
    *   [ ] Implement audit_unknown() trace utility. (Difficulty: 3/10).
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **Engineering Excellence (Reach for More):**
    *   [ ] Implement deferred tag resolution logic. (Difficulty: 3/10).
    *   [ ] Add structure validation for reified Unknown fields. (Difficulty: 3/10).
    *   [ ] Implement bitset-based tag indexing. (Difficulty: 3/10).
    *   [ ] Add CRC32 verification kernel for raw buffers. (Difficulty: 3/10).
    *   [ ] Implement background audit of Unknown blob consistency. (Difficulty: 3/10).
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.