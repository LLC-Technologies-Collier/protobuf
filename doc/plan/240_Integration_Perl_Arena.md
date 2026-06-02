# Milestone 24: Integration - Perl Arena

[TOC]

*   [x] No specific integration tests needed for Arena on its own with prior Perl components, as it's the first.
*   [x] Create and implement `perl/xt/coro/240_arena.t` to test Coro safety.
*   [ ] TODO: Stress concurrent arena allocation across 1000 coroutines.
*   [ ] TODO: Verify arena migration from Coro to Mojo.
*   [x] Create and implement `perl/xt/mojo/240_arena.t` to test Mojo::IOLoop concurrency safety.
*   [x] Integrate Arena stats with Mojo::Log.
*   [ ] Add Mojo::IOLoop timer for periodic arena allocation. (Difficulty: 2/10).
*   [ ] Verify arena registry stability under Mojo context switches. (Difficulty: 3/10).
*   [ ] Create and implement `perl/xt/anyevent/240_arena.t` to test AnyEvent concurrency safety. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**
    *   [ ] Implement 10k coro stress for arena registry. (Difficulty: 3/10).
    *   [ ] Profile registry lock contention under high concurrency. (Difficulty: 3/10).
    *   [ ] Add shared memory metadata for arena migration. (Difficulty: 3/10).
    *   [ ] Implement cross-event-loop cache entry transfer. (Difficulty: 3/10).
    *   [ ] Implement idle-time arena pre-allocation hook. (Difficulty: 3/10).
    *   [ ] Add predictive allocator based on historical usage. (Difficulty: 3/10).
*   [ ] Review and update perl/doc/architecture/** documents based on this milestone's learnings.