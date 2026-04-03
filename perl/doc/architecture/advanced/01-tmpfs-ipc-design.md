# High-Performance Shared Memory IPC Design (tmpfs + SELinux)

## Overview
This document describes the architecture for an ultra-high-performance Inter-Process Communication (IPC) mechanism for Protocol Buffers in Perl. It leverages the Linux kernel's `tmpfs` (memory-backed filesystem) for zero-network-overhead data transfer and SELinux for fine-grained Mandatory Access Control (MAC).

## Architectural Components

### 1. The Transport Layer: `tmpfs`
Instead of serializing messages into a TCP or Unix domain socket, messages are serialized directly into a memory-mapped file residing in a `tmpfs` mount (typically `/dev/shm`).

*   **VPP-Style Graph Handoff:** This mechanism mirrors the **Vector Packet Processor (VPP)** architecture's zero-copy handoff. By moving the pointer to the shared memory block instead of the data itself, we eliminate kernel protocol stack overhead and context switches.
*   **Mapping:** Processes `mmap()` a shared file.
*   **Speed:** Data transfer happens at memory bandwidth speeds.
*   **State:** The file contains the **serialized wire format** of the Protobuf message, not the raw C structures (to avoid address space layout randomization issues with C pointers).

### 2. The Signaling Mechanism: `eventfd` or Unix Sockets
Since `tmpfs` does not notify readers of writes, a lightweight signaling mechanism is required.
*   **Primary choice:** `eventfd` for single-bit synchronization.
*   **Fallback:** A Unix domain socket sending a small notification (e.g., the offset and length of the new message).

### 3. The Security Model: SELinux & POSIX
The "Auth-Aware" nature of this pool is enforced by the kernel.
*   **POSIX Permissions:** Standard `u+r, g+r` bits ensure that only the correct UID/GID can access the file.
*   **SELinux Contexts:** Every memory block (file in `/dev/shm`) is assigned an SELinux label (e.g., `protobuf_auth_t`).
    *   The `auth-daemon` process is granted `read/write` access to `protobuf_auth_t`.
    *   The `web-worker` process is granted `read-only` access to `protobuf_auth_t`.
    *   The kernel blocks any other process from even seeing the existence of the memory block.

## Implementation Plan (Harness)

1.  **Harness Process:** A master Perl script that:
    *   Creates a 1MB file in `/dev/shm/proto_ipc`.
    *   Spawns a **Perl Worker** and a **Python Worker**.
    *   Passes the file descriptor and signaling socket to both.

2.  **Workflow:**
    *   **Perl Worker** populates a complex `TestAllTypes` message.
    *   **Perl Worker** serializes it into the shared memory block.
    *   **Perl Worker** sends a "Ready" signal.
    *   **Python Worker** receives the signal, reads from the shared memory, and parses it.
    *   **Python Worker** validates the contents and sends an "ACK" signal back.

## Fuzzing Strategy
The harness will include a "Chaos Mode" where it writes garbage into the shared memory block to ensure that both the Perl and Python UPB runtimes fail gracefully (croak) instead of segfaulting.

*   **Malformed headers:** Change wire types to invalid values.
*   **Truncated data:** Send signal before write is complete.
*   **Memory corruption:** Randomly flip bits in the shared block.

## Advanced Optimization Goals

To achieve world-class IPC performance and safety, the implementation includes (or is planned to include) the following:

-   **Zero-Copy Deserialization**: Implement `Protobuf::Arena->attach_message(name, offset)` which reifies a message directly from the shared memory block without copying data into intermediate Perl SVs.
-   **Security Verification**: Use `Protobuf::Arena->verify_selinux()` to ensure the shared memory segment complies with security policies before access.
-   **Handle Serialization**: Use `Protobuf::Arena->get_path()` and `Protobuf::Arena->get_offset(ptr)` to exchange message locations between processes.
-   **Cross-Process Identity**: Use `$message->get_fingerprint()` to obtain a unique `path:offset` identifier, allowing disparate processes to confirm they are looking at the same memory instance without expensive deep-equality checks.
-   **Integrated Fuzzer Verification**: (Planned) Every IPC release MUST be verified against a continuous fuzzing harness that simulates malicious shared memory state.
-   **Multi-Language Handshake**: (Planned) Define a standard "shared memory handshake" protocol to allow the Perl implementation to discover and negotiate capabilities with official Python and C++ runtimes sharing the same pool.

## Future Evolution (Reach for More)

-   **NUMA-Aware Allocation Balance**: (Planned) Distribute arena blocks across NUMA nodes based on load to maximize memory bandwidth in high-core systems.
-   **COW-Optimized Shared Cache**: (Planned) Utilize Copy-On-Write (COW) memory mapping for the shared object cache, enabling massive read scaling with near-zero memory footprint for replicated workers.
-   **SIMD-Accelerated Integrity Scanning**: (Planned) Utilize AVX-512 instructions to scan large shared arenas for canary corruption in parallel, minimizing the latency of frequent integrity audits.

## Conclusion
This design provides a blueprint for a production-ready, secure, and extremely fast IPC mechanism that bypasses the limitations of the Perl interpreter's threading model.
