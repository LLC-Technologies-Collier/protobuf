# Milestone 24: Integration - Perl Arena

[TOC]

*   [x] No specific integration tests needed for Arena on its own with prior Perl components, as it's the first.
*   [x] Create and implement `perl/xt/coro/240_arena.t` to test Coro safety.
*   [ ] TODO: Stress concurrent arena allocation across 1000 coroutines.
*   [ ] TODO: Verify arena migration from Coro to Mojo.
*   [x] Create and implement `perl/xt/mojo/240_arena.t` to test Mojo::IOLoop concurrency safety.
*   [x] Integrate Arena stats with Mojo::Log.
*   [ ] TODO: Stress Mojo IOLoop with high-frequency arena allocation. (Difficulty: 6/10).
*   [ ] Create and implement `perl/xt/anyevent/240_arena.t` to test AnyEvent concurrency safety. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Massive Concurrency Arena Stress (10k coroutines) to verify registry scaling. (Difficulty: 7/10).
    *   [ ] TODO: Implement Cross-Event-Loop Arena Migration Safety verification. (Difficulty: 8/10).
    *   [ ] TODO: Implement JIT Arena Pre-fetching integrated with event loop idle cycles. (Difficulty: 9/10).
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.