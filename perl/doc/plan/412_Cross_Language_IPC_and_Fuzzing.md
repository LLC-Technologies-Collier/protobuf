# Milestone 412: Cross-Language IPC, Shared Memory, and Fuzzing (Rust-like Rigor)

[TOC]

## Objective
To guarantee that the Perl UPB implementation is as rock-solid and rigorous as languages like Rust are famous for. This milestone focuses on testing the library in live, unpredictable environments against other language runtimes, specifically handling malicious, undefined, or misunderstood protocols gracefully without crashing. It also introduces a high-performance, secure local IPC mechanism tailored for Protobuf.

## High-Performance Shared Memory IPC (tmpfs + SELinux)
*   [ ] Design an IPC harness that leverages a memory-backed filesystem (`tmpfs`, e.g., `/dev/shm`) to pass serialized Protobuf messages, completely bypassing network socket serialization overhead.
*   [ ] Implement a lightweight signaling mechanism (e.g., Unix domain sockets or `eventfd`) to notify peers when a serialized message is ready in `tmpfs`.
*   [ ] **Auth-Aware Isolation:** Configure standard POSIX permissions and SELinux Mandatory Access Control (MAC) contexts to restrict read/write access to specific `tmpfs` blocks. Ensure that only authorized processes (e.g., `auth-daemon` vs `www-data`) can write to or read sensitive auth tokens, demonstrating hardware-level page fault security mediated by the kernel.
*   [ ] Spawn two concurrent processes: one using the **Perl** UPB runtime, and the other using the **Python** UPB runtime.
*   [ ] Verify absolute 1:1 agreement on the structure and content of messages transferred via the `tmpfs` pool.

## Robustness and Fuzzing (Undefined Situations)
*   [ ] Write tests that intentionally exercise "undefined situations" (e.g., malformed headers, invalid wire types, deeply nested garbage data mapped into the `tmpfs` block).
*   [ ] **Graceful Degradation:** Implement and test logic where the Perl client/server fails safely (e.g., cleanly croaking or returning a Perl-level error) when parsing malicious data, without segfaulting or crashing the interpreter.
*   [ ] **Connection/State Reset:** If the peer fails to recover or continues mapping garbage data, cleanly reset the connection and ensure no UPB Arenas or memory structures are leaked.
*   [ ] Ensure the Perl interpreter survives all Fuzzing attempts without segfaults (leveraging ASan to verify).

*Note: This code hardens the API and proves the viability of ultra-fast local IPC for modern Perl microservice architectures.*