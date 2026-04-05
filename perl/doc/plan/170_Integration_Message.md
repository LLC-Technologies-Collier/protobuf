# Milestone 17: C Layer - Integration Tests (message + previous)

[TOC]

*   [x] Create test file `perl/t/c/integration/170_message.c`.
*   [x] Tests cover message interactions with all other components. (Verified accessors, serialize, parse, compare, and deterministic serialization).
*   [x] Implement Deterministic Serialization integration test.
*   [ ] Add state transition metadata for Oneof fields. (Difficulty: 3/10).
*   [ ] Verify Oneof invalidation in concurrent contexts. (Difficulty: 3/10).
*   [ ] Implement bulk field copy utility for parse_from. (Difficulty: 3/10).
*   [x] Add performance benchmark for deep message merging.
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/170_message_coro.c`
*   [ ] Tests in `perl/t/c/integration/170_message_coro.c` use libcoro to stress concurrent C function usage for message. (Partially implemented).
*   [ ] Add libcoro stress for message reserialization. (Difficulty: 3/10).
*   [ ] Implement fallback to transient arenas during pressure. (Difficulty: 3/10).
*   [ ] Verify cache HIT ratio during high-frequency construction. (Difficulty: 3/10).
*   [ ] Add thread-local locks for shared message wrappers. (Difficulty: 3/10).
*   [ ] Implement TSAN-style race detection for internal message state. (Difficulty: 3/10).
*   [ ] Integrate race checks into concurrent message tests. (Difficulty: 3/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**
    *   [ ] Implement vectorized field-by-field comparison. (Difficulty: 3/10).
    *   [ ] Add AVX2 kernel for message tree equality. (Difficulty: 3/10).
    *   [ ] Implement idle-time message pre-allocation hook. (Difficulty: 3/10).
    *   [ ] Add predictive field pre-fetcher for hot message paths. (Difficulty: 3/10).
    *   [ ] Implement audit_integrity() for message structures. (Difficulty: 3/10).
    *   [ ] Add background audit of structure and canary consistency. (Difficulty: 3/10).
