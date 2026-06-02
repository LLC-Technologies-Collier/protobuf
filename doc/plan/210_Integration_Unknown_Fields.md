# Milestone 21: C Layer - Integration Tests (unknown_fields + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/210_unknown_fields.c`.
*   [x] Tests cover unknown fields interactions with messages. (Verified parsing, retrieval, reserialization, and manual addition).
*   [x] Add field scrubbing infrastructure to UnknownFieldSet.
*   [x] Implement single-tag selective removal.
*   [ ] Add upb_MessageDef verification to Unknown promotion. (Difficulty: 3/10).
*   [ ] Implement safety checks for reifying Unknown blobs. (Difficulty: 3/10).
*   [ ] Add test for unknown field preservation in nested merges. (Difficulty: 2/10).
*   [ ] Verify unknown field metadata during merge cycles. (Difficulty: 3/10).
*   [ ] Add libcoro stress for unknown field growth. (Difficulty: 3/10).
*   [ ] Implement thread-local locks for Unknown reification. (Difficulty: 3/10).
*   [ ] Verify cache hit rate during concurrent unknown access. (Difficulty: 3/10).
*   [ ] Implement TSAN-style race detection for internal Unknown state. (Difficulty: 3/10).
*   [ ] Integrate race checks into concurrent unknown field tests. (Difficulty: 3/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] Implement bulk-tag removal using a bitmask. (Difficulty: 3/10).
    *   [ ] Add test for multi-tag removal performance. (Difficulty: 2/10).
    *   [ ] Implement reflective validation for promoted unknown fields. (Difficulty: 3/10).
    *   [ ] Add cross-pool resolution for promoted message types. (Difficulty: 3/10).
    *   [ ] Add vectorized tag identification in binary buffers. (Difficulty: 3/10).
    *   [ ] Implement bulk-tree scanner using SSE4.1. (Difficulty: 3/10).

