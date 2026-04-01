# Milestone 12: C Layer - extension_dict

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/extension_dict/dict.c:**
    *   [x] Create test file `perl/t/c/extension_dict/dict.c`.
    *   [ ] Tests cover all public functions. (Current: failing class verification and parent retrieval).
    *   [x] Implement C functions.
    *   [ ] C tests pass. (Current: Leak detected in PerlUpb_ExtensionDict_New).
*   **perl/xs/extension_dict/iterator.c:**
    *   [x] Create test file `perl/t/c/extension_dict/iterator.c`. (TODO stub created).
    *   [ ] Tests cover all public functions.
    *   [x] Implement C functions.
    *   [ ] C tests pass.

## Reach for More (World-Class Goals)
*   [ ] TODO: Implement Lazy Extension Resolution (deferred descriptor inflation).
*   [ ] TODO: Verify stable identity for extension objects across multiple iterations.
*   [ ] TODO: Implement safe cross-message extension migration semantics for shared arenas.
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
