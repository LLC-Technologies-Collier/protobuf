# XS and C-Layer Development Guidelines

This document outlines the specific conventions and testing patterns for the XS and C-layer implementation of this project.

## Directory Structure
- `perl/lib/`: Contains `.pm` modules and their corresponding `.xs` bindings.
- `perl/xs/`: Contains pure C logic (source and headers) that are linked into the common shared library or used by the XS bindings.
- `perl/t/c/`: Contains C-level unit tests.
- `perl/xt/`: Contains extended integration tests (e.g., concurrency testing with Coro/Mojo).

## Planning Documents
- All planning documents in `perl/doc/plan/` MUST use paths relative to the project root (e.g., `perl/xs/descriptor.c`) to avoid ambiguity.
- Milestones are not considered complete until literal compliance (existence of files, passing tests, specified functionality) is verified on disk.

## Testing Conventions
### C-Level Unit Tests
- Use the test harness provided in `perl/t/c/upb-perl-test.h`.
- Follow the TAP (Test Anything Protocol) format.
- **Indentation**: TAP 13 compliant indented output is supported for logical grouping of tests.
- **TODO Blocks:** Use the `TODO("reason") { ... }` macro to track planned coverage that is not yet implemented.
- **Subtests:** Use the `subtest("name", { ... })` macro for hierarchical test organization.
- **Leak Checking:** Use the `LEAK_CHECK { ... }` block to wrap logic that MUST be allocation-neutral.
- **Thread Stress**: Use `STRESS_THREADS(n, func, arg)` to verify concurrency safety (currently a stub).
- **XS-Based C Runner:** For C logic that requires a Perl interpreter context (`aTHX`), use the XS runner defined in `perl/t/c/Test.xs`.

### Concurrency and Thread Safety
- **Striped Mutexes:** For high-contention resources (like caches), utilize the striped mutex implementation in `obj_cache.c`.
- **Lock Ordering:** Maintain a strict lock acquisition order (e.g., Global/Audit Lock -> Stripe Lock) to prevent deadlocks.
- **Thread Stability:** Inside thread-stress functions, always wrap Perl-related logic in `ENTER; SAVETMPS; ... FREETMPS; LEAVE;` blocks.
- **Pointer Recovery:** Use `sscanf(key, "%p", &ptr)` to reliably recover original C pointers from stringified hash keys.

## Multi-Module XS Conventions
- **Colocated Files:** Every package requiring XS exports (e.g., `Protobuf::Internal`) MUST have a corresponding `.xs` and `.pm` file.
- **Initialization:** Use a centralized `PerlUpb_Protobuf_InitModule(aTHX)` function called from the `BOOT:` section of the main module's `.xs` file.
- **Per-Interpreter Registry:** All interpreter-local C state (Caches, Audit Logs, etc.) MUST be stored in the centralized registry struct (`PerlUpb_Registry`) retrieved via `PerlUpb_Registry_Get(aTHX)`. Avoid direct usage of `get_sv` in performance-critical paths.
- **Boot Alignment:** Ensure `MODULE` and `PACKAGE` declarations in `.xs` files exactly match the Perl package name to avoid symbol lookup errors.

## Vectorized Processing (VPP Style)
- **Instruction Cache (I-cache) Optimization:** Prefer processing batches (vectors) of messages through a single C logic path before switching tasks. This minimizes I-cache thrashing, a key performance principle from the **Vector Packet Processor (VPP)**.
- **Data Cache (D-cache) Warming:** Utilize thread-local "fast-path" arenas to ensure that the memory addresses used for high-frequency small messages remain warm in the L1/L2 data cache.
- **SIMD-Accelerated Batching:** Implementation of bulk scanners or parsers should leverage the linear memory layout of vectorized batches to utilize SIMD (SSE4.2/AVX2) for tag searching and UTF-8 validation.
- **Batch Parse API:** High-performance interfaces SHOULD support taking a vector (array) of binary blobs and returning a batch of reified objects to maximize throughput.

## Python Parity
- The primary goal of this implementation is full feature parity with the Python upb-based implementation.
- Alignment with Python's behavior for `AddSerializedFile`, `DescriptorPool` resolution, and `SymbolDatabase` is mandatory.
- **Deterministic Serialization**: When requested by the user or required for stable signatures, serialization logic MUST provide consistent field ordering.
- **Efficient Merging**: Merging logic (`parse_from`) MUST be architected to leverage arena sharing where possible to minimize redundant allocations.
- **COW for Containers**: (Planned) Large repeated and map fields should utilize Copy-On-Write (COW) semantics during message cloning to avoid expensive memory operations.
- **SIMD Optimization**: Performance-critical operations on numeric repeated fields (appending, scanning, sorting) should follow established SIMD patterns (SSE4.1/AVX2) for world-class throughput.
- **Integrated Deduplication**: (Planned) Container merging logic should support high-performance C-level deduplication to ensure data integrity during complex message merging.
- **Concurrency Pressure**: Integration tests for containers MUST include stress testing under memory pressure and high-frequency reallocation to verify the robustness of the shared arena model.
- **Unknown Field Performance**: (Planned) High-performance analysis of unknown fields should utilize internal C-level indexing to avoid redundant buffer scans.
- **Observability Audit**: Every core component (Cache, Arena, Unknown Fields) should support detailed trace logging for advanced debugging.
- **Chaos Integration Testing**: Final C-layer integration MUST include randomized, high-concurrency mutation of all supported field types (scalars, repeated, maps, extensions, unknown fields) in a single message context.
- **Schema Evolution Safety**: Integration tests MUST verify system stability when parsing data into evolved message definitions with mismatched or missing fields.
- **Global Lifecycle Audit**: (Planned) Implement C-level infrastructure to trace the entire lifecycle of message and arena objects across multiple mutation events.
- **Arena Fusion Safety**: (Planned) Implementations supporting arena fusion MUST ensure that all dependent objects (Messages, Descriptors) correctly transition to the new shared arena lifecycle without memory corruption.
- **Custom Allocator Support**: (Planned) The `upb_Arena` abstraction MUST be extensible to support pluggable allocation backends for specialized memory management needs.
- **Lock-Free Descriptor Retrieval**: Descriptor search logic MUST be optimized for high-frequency concurrent access (e.g., in Coro/Mojo) to avoid serialization bottlenecks.
- **Dynamic Reloading Safety**: (Planned) Implementations of `DescriptorPool` that support reloading MUST ensure that old definitions remain valid as long as any `upb_Message` instance references them.
- **Fast-Path Descriptor Access**: Internal C-layer tasks requiring descriptor information SHOULD utilize direct access to `upb_Def` structures to avoid the overhead of Perl object construction in performance-critical paths.
- **Stable Schema Fingerprinting**: Descriptor definitions SHOULD support deterministic hashing to facilitate efficient schema-registry lookups across different environments.
- **Zero-Copy Field Access**: (Planned) Implementations for large string and bytes fields SHOULD provide a zero-copy path that exposes the underlying `upb` memory directly to Perl.
- **Moo Dispatch Bypass**: (Planned) High-frequency field accessors SHOULD be optimized to bypass the Perl-level method dispatch mechanism and call the underlying C implementation directly.
- **SIMD Bulk Transfer (Perl-to-C)**: (Planned) XS bindings for container types MUST optimize bulk data transfer from Perl arrays/hashes to `upb` structures using SIMD-accelerated C functions.
- **Native XS Container Operations**: (Planned) Transformation operations like `grep` and `map` SHOULD be implemented directly in the XS layer for tied containers to minimize Perl-to-C transition overhead.
- **Bulk Map Projection**: (Planned) Implementations for map fields MUST support an O(1) projection path to convert a `upb_Map` into a native Perl hash for high-throughput bulk reads.
- **Key Deduplication Safety**: (Planned) Arena-level key deduplication MUST maintain strict immutability of shared keys to prevent side-effects across different map instances.
- **Vectorized WKT Conversions**: (Planned) Performance-critical Well-Known Types (Timestamp, Duration) SHOULD utilize vectorized C functions (SSE4.1/AVX2) for data conversion to support high-throughput analytics.
- **Fast-Path Any Unpacking**: (Planned) Implementations of `google.protobuf.Any` SHOULD provide a C-layer path for unpacking that avoids Perl object overhead for internal routing tasks.
- **Direct-to-Stream Text Printing**: (Planned) Large TextFormat outputs SHOULD be printed directly to C-level streams or file descriptors to avoid Perl string allocation overhead.
- **Integrated Data Redaction**: (Planned) Serialization and TextFormat logic SHOULD support pluggable redaction filters to prevent sensitive data exposure in logs.
- **Direct JSON-to-Wire Conversion**: (Planned) Large JSON inputs SHOULD be parsed directly into wire-format buffers using C-layer logic to minimize intermediate object creation.
- **Streaming JSON Serialization**: (Planned) JSON generation for large messages SHOULD support streaming directly to C-level streams or file descriptors.
- **IPC Safety (tmpfs)**: (Planned) High-performance IPC implementations using shared memory MUST utilize kernel-level isolation (POSIX/SELinux) and verify message integrity against malicious peers.
- **Fuzzer-Driven Development**: (Planned) Security-critical C-layer components (Parsers, Converters, IPC) SHOULD be integrated with a continuous fuzzing harness to identify edge-case vulnerabilities.
- **ithread Safety (CLONE)**: Every Perl class wrapping a C pointer MUST implement a `CLONE` method that explicitly prevents unsafe shallow-copying across `ithread` boundaries until a safe global-sharing mechanism is implemented.
- **TSan-Driven Development**: (Planned) Multi-interpreter integration tests MUST be executed under ThreadSanitizer (TSan) to identify and eliminate data races in global or shared C state.
- **protoc Plugin Alignment**: (Planned) The `protoc` code generator SHOULD utilize the same C-layer descriptor logic as the runtime library to ensure absolute consistency in generated code.
- **Automated Async Services**: (Planned) Service definitions in Protobuf SHOULD result in automated generation of asynchronous stubs for supported Perl event loops (Mojo, Coro).
- **Automated Quality Gates**: (Planned) Every release MUST pass automated quality gates including ASan leak detection, TSan race detection, and fuzzer verification.
- **Embedded Performance Verification**: (Planned) The distribution SHOULD include tools for users to verify library performance and correctness in their local environment.
- C-layer utilities should be designed to support the eventual high-level Python-like APIs in Perl.

## Memory Safety and Debugging
- All tests MUST pass under **AddressSanitizer (ASan)**.
- Use `detect_leaks=1` in `ASAN_OPTIONS` during local development to catch memory leaks early.
- Weak references in the object cache MUST be rigorously validated to prevent stale pointer usage.
