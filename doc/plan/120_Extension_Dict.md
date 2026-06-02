# Milestone 12: C Layer - extension_dict

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/extension_dict/dict.c:**
    *   [x] Create test file `perl/t/c/extension_dict/dict.c`.
    *   [x] Tests cover all public functions. (Verified class and parent retrieval).
    *   [x] Implement C functions.
    *   [x] C tests pass. (Fixed leak and parent identity check).
*   **perl/xs/extension_dict/iterator.c:**
    *   [x] Create test file `perl/t/c/extension_dict/iterator.c`. (TODO stub created).
    *   [ ] Tests cover all public functions.
    *   [x] Implement C functions.
    *   [ ] C tests pass.

## Reach for More (World-Class Goals)
*   [x] Add deferred descriptor lookup for extension tags.
*   [x] Implement lazy inflation of extension wrappers.
*   [ ] TODO: Verify stable identity for extension objects across multiple iterations. (Difficulty: 4/10).
*   [ ] Add shared memory metadata for extension arenas. (Difficulty: 3/10).
*   [ ] Implement cross-message SV transfer for extensions. (Difficulty: 3/10).
*   [ ] Implement hash-table for O(1) extension resolution. (Difficulty: 3/10).
*   [x] Add background audit of extension wrapper identity.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
