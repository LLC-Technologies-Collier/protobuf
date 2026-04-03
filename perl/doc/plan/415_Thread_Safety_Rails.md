# Milestone 415: Thread Safety Rails (ithreads)

[TOC]

## Objective
Perl's `threads.pm` (`ithreads`) performs a deep clone of the interpreter, which shallow-copies underlying C pointers in our XS objects. Since UPB arenas are strictly single-threaded and not protected by mutexes, multiple threads writing to the same arena will corrupt the C heap and result in double-free segfaults when the threads exit. 

Instead of attempting the immense performance penalty of deeply serializing/deserializing messages and re-compiling the descriptor pool across thread boundaries, we will follow modern Perl best-practices (Pre-forking and Event Loops) and implement an explicit safety rail to fail loudly if `threads->create` is invoked on a Protobuf object.

## Tasks
*   [x] Implement a `CLONE` method in `Protobuf::Message` (Perl or XS layer).
*   [x] Implement a `CLONE` method in `Protobuf::DescriptorPool` (Perl or XS layer).
*   [x] The `CLONE` methods must explicitly `croak()` with a clear error message. The message should explain that Protobuf objects cannot be shared across `ithread` boundaries and direct users to use pre-forking (which leverages Copy-On-Write memory for the `upb_DefPool`) or event loops instead.
*   [x] Write `t/15-threads.t` to verify that passing a message to a new thread dies safely.
*   [x] Implement Thread-Safe Global Freezing for DescriptorPools.
*   [ ] TODO: Provide high-level utility for Cross-Thread Message Handoff:
    *   [ ] TODO: Implement `Protobuf::Handoff` utility for serializing messages to `threads::shared` queues. (Difficulty: 2/10)
    *   [ ] TODO: Create XS-optimized `freeze`/`thaw` for thread handoff. (Difficulty: 3/10)
    *   [ ] TODO: Implement memory-efficient buffer recycling for handoff queues. (Difficulty: 2/10)
*   [ ] TODO: Integrate ThreadSanitizer (TSan) for automated race detection:
    *   [ ] TODO: Configure Bazel/Makefile build flags for TSan instrumented Perl/XS. (Difficulty: 3/10)
    *   [ ] TODO: Create TSan suppression file for known-safe Perl interpreter internals. (Difficulty: 3/10)
    *   [ ] TODO: Integrate TSan-enabled test run into CI pipeline. (Difficulty: 2/10)

*   **Engineering Excellence (Reach for More):**
    *   [ ] TODO: Implement Cross-Interpreter Object Migration via Shared Memory segments:
        *   [ ] TODO: Design shared-memory layout for cross-interpreter arena migration. (Difficulty: 3/10)
        *   [ ] TODO: Implement XS-level `shmget`/`shmat` wrappers for arena segments. (Difficulty: 3/10)
        *   [ ] TODO: Create pointer-swizzling logic for absolute address resolution in shared segments. (Difficulty: 3/10)
    *   [ ] TODO: Implement Multi-Interpreter Registry Synchronization for shared pool state:
        *   [ ] TODO: Implement C-level mutex-protected registry for shared DescriptorPools. (Difficulty: 3/10)
        *   [ ] TODO: Create Perl-level proxy objects for synchronized pool access. (Difficulty: 3/10)
        *   [ ] TODO: Implement atomic reference counting for cross-interpreter shared pools. (Difficulty: 2/10)
    *   [ ] TODO: Implement Automated Concurrency Stress-Fuzzer for integrated core:
        *   [ ] TODO: Implement `Protobuf::Stress::Fuzzer` for concurrent message mutation. (Difficulty: 3/10)
        *   [ ] TODO: Create randomized task scheduler for multi-threaded access patterns. (Difficulty: 2/10)
        *   [ ] TODO: Integrate fuzzer with ASan/TSan for crash detection. (Difficulty: 2/10)
*   [x] Review and update perl/doc/architecture/api/01-descriptor-pool.md and core/c-perl-interface.md documents.