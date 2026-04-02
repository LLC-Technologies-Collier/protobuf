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
    *   [ ] TODO: Implement Direct Unknown-to-Message conversion. (Difficulty: 6/10).

    *   [ ] TODO: Implement high-performance Unknown Field Indexing. (Difficulty: 7/10).
    *   [ ] TODO: Implement Trace/Audit for unknown blobs. (Difficulty: 5/10).
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Lazy Unknown Field Parsing for deferred structure reification. (Difficulty: 7/10).
    *   [ ] TODO: Implement O(1) Unknown Field Number Index for ultra-fast tag lookups. (Difficulty: 7/10).
    *   [ ] TODO: Implement Self-Healing Unknown Blob Consistency via CRC32 auditing. (Difficulty: 8/10).
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.