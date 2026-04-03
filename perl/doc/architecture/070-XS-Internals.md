# Architecture: XS Internals & C API

[TOC]

## Purpose of `libprotobufperl.so`

`libprotobufperl.so` is a shared library containing the `upb` core, third-party dependencies (`utf8_range`), and custom C helper functions specifically for this Perl-UPB binding. This library centralizes common logic used across all XS sub-modules (`Arena.so`, `Message.so`, etc.).

## Symbol Visibility and ABI Stability

To ensure ABI stability and prevent symbol collisions, we use a linker version script (`libprotobufperl.map`) to strictly control which symbols are exported from `libprotobufperl.so`.

*   **Global Symbols:** Only symbols within the `PerlUpb_*`, `upb_*`, `_upb_*`, `google_*`, and `utf8_range_*` namespaces are exported.
*   **Local Symbols:** All other symbols (including internal helpers and leaked `main` functions from test objects) are hidden from the global namespace using `local: *;`.

## C API Naming Conventions for XS

*   Functions intended for global export MUST be prefixed with `PerlUpb_`.
*   Functions intended to be called from XS should follow the `PerlUpb_<Component>_<Method>` pattern.
*   The top-level initialization function is `PerlUpb_Protobuf_InitModule(pTHX)`.

## Initialization Flow

1.  **XS Loading:** When the main `Protobuf.so` or any sub-module is loaded, it links against `libprotobufperl.so`.
2.  **BOOT Section:** The `BOOT:` section in `lib/Protobuf.xs` MUST call `PerlUpb_Protobuf_InitModule(aTHX)`.
3.  **Component Registration:** `PerlUpb_Protobuf_InitModule` initializes the **Per-Interpreter Registry**, object cache, audit log, and registers internal XS functions in the `Protobuf::Internal` namespace.

## Per-Interpreter Registry

To avoid expensive global Perl SV lookups (`get_sv`) in performance-critical C paths, the implementation utilizes a centralized registry struct (`PerlUpb_Registry`) stored in the interpreter's `PL_modglobal` hash.

*   **Header:** `xs/protobuf/registry.h`
*   **State Managed:** Object Cache (`HV*`), LRU List (`AV*`), Audit Log (`void*`), and global configuration (e.g., `max_cache_capacity`).
*   **Access:** XS functions SHOULD use `PerlUpb_Registry_Get(aTHX)` to retrieve the current interpreter's state. This pattern de-risks future feature implementation (like thread-local arena caching) by providing a single, type-safe C hook for all global state.

## Lock Contention Profiling

To ensure scalability in multi-threaded environments, the striped object cache mutexes include built-in contention profiling.

*   **Implementation:** `LOCK_AND_PROFILE` macro utilizes `pthread_mutex_trylock` to detect immediate availability.
*   **Metrics:** Tracks `acquisitions` and `contentions` per stripe, as well as for global `lru` and `audit` locks.
*   **Visibility:** Stats are exposed to Perl via `Protobuf::Internal::get_contention_stats()`.

## Predictive Allocator

To minimize expensive reallocations for recurring message patterns, the `StatsAlloc` engine tracks peak memory usage.

*   **Mechanism:** Maintains a `historical_max_size` per `StatsAlloc` instance.
*   **Optimization:** When acquiring a new arena through the Registry, the `historical_max_size` is used as a hint for `upb_Arena_Init` (capped at 1MB), pre-allocating a single block large enough for the expected workload.

## Automated Race Detection (TSAN)

The project includes integrated support for ThreadSanitizer (TSAN) to detect data races in the XS core and concurrent C integration tests.

*   **Usage:** Run `make test_tsan` to rebuild the library with `-fsanitize=thread` and execute the full test suite.
*   **Scope:** Validates thread-safety of the per-interpreter registry, object cache stripes, and shared memory arena accesses.

## Generalized Block Allocators

To support high-performance allocation patterns (like zero-copy IPC and thread-local caching), we utilize a generalized block allocator (`PerlUpb_BlockAlloc`).

*   **Header:** `xs/protobuf/arena.h` (Internal logic in `xs/protobuf/arena_tmpfs.c`)
*   **Abstraction:** Wraps a contiguous memory region and provides a `upb_alloc` compliant interface.
*   **Backends:** Supports `PERL_UPB_BLOCK_MMAP` (for file-backed shared memory) and `PERL_UPB_BLOCK_MALLOC` (for RAM-backed local memory).
*   **Usage:** Used by the Arena Factory to acquire arenas with specific performance characteristics (e.g., `PerlUpb_Arena_NewBlock`).

## Memory Canaries

To detect memory corruption (buffer overflows/underflows) in performance-critical C paths, all custom allocators (`StatsAlloc` and `BlockAlloc`) implement canary guards.

*   **Pattern:** `0xDEADBEEFCAFEBABEULL` (16 bytes at start and end).
*   **Verification:** Performed automatically during `free`, `realloc`, and arena destruction.
*   **Safety:** Errors trigger a Perl `croak` with a descriptive message (e.g., "MEMORY CORRUPTION DETECTED (Overflow)").

## SIMD Acceleration (VPP Alignment)

Aligning with the **Vector Packet Processor (VPP)** philosophy, hot paths utilize SIMD instructions where appropriate to maximize throughput.

*   **CPUID Dispatcher:** `PerlUpb_InitCpuFeatures()` detects hardware capabilities at runtime. Optimized kernels are selected dynamically based on available instruction sets (SSE4.1, AVX2).
*   **Name Conversion:** `PerlUpb_ClassNameToFullName` utilizes AVX2/SSE4.1 instructions to process class names in bulk, accelerating the common case where no special characters are present.
*   **Batch Validation:** `PerlUpb_FieldVector` provides a C-level API for collecting field descriptors and values into contiguous vectors, enabling vectorized validation kernels (e.g., `PerlUpb_ValidateIntRange_SSE41`).
*   **Hardware Requirement:** Implementation includes scalar fallbacks for all optimized paths to ensure portability across heterogeneous environments.

## Typemap Strategy

*   **Location:** Typemap entries will be placed in the `perl/typemap` file.
*   **Organization:** Group typemaps by the C type they handle.
*   **Naming:** Use standard typemap conventions.
*   **Usage:** Utilize typemaps to automate the conversion between Perl SVs and C types like `upb_Arena *`, `upb_Message *`, `upb_MiniTable *`, etc., and various descriptor types.

## Data Passing Between Perl and C

*   **Input to C:** Functions taking string data from Perl should expect to receive a `char *` and potentially a length, typically derived from `SvPV`. The C function is responsible for copying the data into an arena if it needs to persist beyond the function call.
*   **Output to Perl:** Functions returning data to Perl should generally return new SVs created via `newSVpv`, `newSVuv`, etc., with their ownership passed to Perl. Memory for these SVs is managed by Perl's aREFCNT system.
