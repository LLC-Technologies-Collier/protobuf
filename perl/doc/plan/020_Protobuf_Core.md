# Milestone 2: C Layer - protobuf (Core Utilities)

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **xs/protobuf/obj_cache.c:**
    *   [ ] Create test file `t/c/protobuf/obj_cache.c`.
    *   [ ] Tests in `t/c/protobuf/obj_cache.c` test all public functions in `xs/protobuf/obj_cache.h` (creation, insertion, lookup, deletion, clearing), including checks for re-entrancy and safe state management.
    *   [ ] Implement C functions in `xs/protobuf/obj_cache.c`.
    *   [ ] C tests pass for `t/c/protobuf/obj_cache.c`.

*   **xs/protobuf/arena.c:**
    *   [ ] Create test file `t/c/protobuf/arena.c`.
    *   [ ] Tests in `t/c/protobuf/arena.c` test all public functions in `xs/protobuf/arena.h` (creation, free, etc.), including checks for re-entrancy and safe state management.
    *   [ ] Implement C functions in `xs/protobuf/arena.c`.
    *   [ ] C tests pass for `t/c/protobuf/arena.c`.

*   **xs/protobuf/utils.c:**
    *   [ ] Create test file `t/c/protobuf/utils.c`.
    *   [ ] Tests in `t/c/protobuf/utils.c` test all public functions in `xs/protobuf/utils.h`, including checks for re-entrancy and safe state management.
    *   [ ] Implement C functions in `xs/protobuf/utils.c`.
    *   [ ] C tests pass for `t/c/protobuf/utils.c`.
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.
