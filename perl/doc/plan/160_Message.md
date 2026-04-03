# Milestone 16: C Layer - message

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/message/access.c:**
    *   [x] Create test file `perl/t/c/message/access.c`. (Covered in perl/t/c/message/message.c)
    *   [x] Tests cover all public functions in `xs/message/access.h`.
    *   [x] Implement C functions.
    *   [x] C tests pass.
*   **perl/xs/message/compare.c:**
    *   [x] Create test file `perl/t/c/message/compare.c`. (Covered in perl/t/c/message/message.c)
    *   [x] Tests cover all public functions in `xs/message/compare.h`.
    *   [x] Implement C functions.
    *   [x] C tests pass.
*   **perl/xs/message/init.c:**
    *   [ ] Create test file `perl/t/c/message/init.c`.
    *   [ ] Tests cover all public functions in `xs/message/init.h`.
    *   [ ] Implement C functions.
    *   [ ] C tests pass.
*   **perl/xs/message/message.c:**
    *   [x] Create test file `perl/t/c/message/message.c`.
    *   [x] Tests cover all public functions in `xs/message/message.h`. (Verified wrapper creation, class blessing, and property retrieval).
    *   [ ] TODO: Implement SIMD-accelerated serialization for fixed-length types.
    *   [ ] TODO: Implement C-level MiniTable reflection cache.
    *   [ ] TODO: Implement COW (Copy-On-Write) semantics for sub-messages.
    *   [x] Implement C functions.
    *   [x] C tests pass.
*   **perl/xs/message/meta.c:**
    *   [x] Create test file `perl/t/c/message/meta.c`. (Covered in perl/t/c/message/message.c)
    *   [x] Tests cover all public functions in `xs/message/meta.h`.
    *   [x] Implement C functions.
    *   [x] C tests pass.
*   **perl/xs/message/serialize.c:**
    *   [x] Create test file `perl/t/c/message/serialize.c`. (Covered in perl/t/c/message/message.c)
    *   [x] Tests cover all public functions in `xs/message/serialize.h`.
    *   [x] Implement C functions.
    *   [x] C tests pass.
*   **perl/xs/message/wkt.c:**
    *   [ ] Create test file `perl/t/c/message/wkt.c`.
    *   [ ] Tests cover all public functions in `xs/message/wkt.h`.
    *   [ ] Implement C functions.
    *   [ ] C tests pass.

*   **Engineering Excellence (Reach for More):**
    *   [ ] Implement multi-message parsing orchestration in C. (Difficulty: 3/10).
    *   [ ] Add vectorized dispatch for incoming binary buffers. (Difficulty: 3/10).
    *   [ ] Add support for arena-relative pointers in message structures. (Difficulty: 3/10).
    *   [ ] Implement shared memory metadata tracking for IPC arenas. (Difficulty: 3/10).
    *   [ ] Integrate NUMA node identification in `upb_alloc` overrides. (Difficulty: 3/10).
    *   [ ] Verify message tree placement on target memory nodes. (Difficulty: 3/10).
*   [x] Review and update perl/doc/architecture/** documents based on this milestone's learnings.