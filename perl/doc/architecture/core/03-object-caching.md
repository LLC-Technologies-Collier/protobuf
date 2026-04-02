# Object Caching

_Status: Fully Implemented_

To ensure object identity and improve performance, the implementation uses a global (per-interpreter) cache to map underlying UPB C objects (pointers) to their corresponding Perl wrapper SVs.

## Cache Mechanism

The C implementation of the object cache is located in `perl/xs/protobuf/obj_cache.c` and `perl/xs/protobuf/obj_cache.h`.

*   **Registry-Based Storage:** To eliminate expensive global Perl SV lookups (`get_sv`), the Object Cache (`HV*`), LRU array (`AV*`), and Audit Log are stored within a centralized **Per-Interpreter Registry** (`PerlUpb_Registry`).
*   **Keys:** Hexadecimal string representations of C pointer addresses (e.g., `0x7fd1a2b3c4d5`).
*   **Values:** Weak references (using `sv_rvweaken`) to the blessed Perl objects.

## Concurrency and Scalability

The cache is designed for high-throughput, multi-threaded environments (ithreads/Coro):

1.  **Striped Locking:** Replaces a single global lock with a **16-stripe mutex** array. Pointers are hashed to a specific stripe to minimize contention during concurrent access.
2.  **Lock Abstraction:** Uses `perl/xs/protobuf/port.h` to provide portable mutex macros (`PERL_PROTOBUF_MUTEX_LOCK`, etc.) that utilize Perl's native mutexes when `USE_ITHREADS` is defined.
3.  **Interpreter Isolation:** The Registry is stored in the interpreter's `PL_modglobal` hash. This ensures that every thread/coroutine has zero-contention access to its own state.

## LRU Eviction

To prevent unbounded memory growth, the cache implements a FIFO-based eviction strategy:

*   **Capacity:** Configurable via `Protobuf::Internal::set_cache_capacity()` (Default: 100,000).
*   **LRU Tracking:** A Perl array (`AV*`) in `Protobuf::_obj_lru` tracks insertion order.
*   **Eviction Loop:** When capacity is exceeded, the oldest keys are shifted from the array and deleted from the hash. The logic is stripe-aware and verifies existence before eviction to handle stale entries safely.

## High-Performance Audit Log

The cache includes a per-interpreter circular ring buffer for real-time observability:

*   **Event Types:** `ADD`, `HIT`, `MISS`, `DELETE`, `EVICT`.
*   **Performance:** O(1) logging with fixed memory overhead (~32KB per interpreter).
*   **Access:** Exposed via `Protobuf::Internal::get_cache_audit_log()`.

## Usage in XS

Utilize the `RETURN_CACHED_OR_CREATE_BLESSED` macro in `xs/descriptor/base.h` or `PerlUpb_WrapArenaBoundObject` in `xs/protobuf/utils.c` to automatically handle cache lookups and insertions.
