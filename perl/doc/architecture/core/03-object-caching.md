# Object Caching

_Status: C Layer Implemented_

To ensure object identity and improve performance, the implementation uses a global (per-interpreter) cache to map underlying UPB C objects (pointers) to their corresponding Perl wrapper SVs. This mechanism is critical for maintaining consistent Perl object identity and efficient memory management. Verified consistent object identity for wrapped messages in C integration tests.

## Cache Mechanism

The C implementation of the object cache is located in `perl/xs/protobuf/obj_cache.c` and `perl/xs/protobuf/obj_cache.h`.

*   **Global Cache:** A single `HV*` (Perl Hash) is initialized during module load.
*   **Keys:** Hexadecimal string representations of C pointer addresses (e.g., `0x7fd1a2b3c4d5`).
*   **Values:** Weak references (using `sv_rvweaken`) to the blessed Perl objects. This ensures that the cache itself does not extend the lifetime of the Perl wrappers.

## API Functions

-   `void PerlUpb_ObjCache_Add(pTHX_ const void* ptr, SV* obj)`: Adds a Perl object to the cache for the given C pointer. The reference in the cache is weakened.
-   `SV* PerlUpb_ObjCache_Get(pTHX_ const void* ptr)`: Retrieves the Perl object associated with the C pointer. Returns `NULL` if not found or if the weak reference has been collected.
-   `void PerlUpb_ObjCache_Delete(pTHX_ const void* ptr)`: Removes the entry for the given C pointer from the cache.

## Usage in XS

Any function that needs to return a Perl wrapper for a `upb` object MUST:

1.  Call `PerlUpb_ObjCache_Get(aTHX_ ptr)`.
2.  If it returns a valid SV, increment its reference count and return it.
3.  If it returns `NULL`, create the new Perl wrapper SV, call `PerlUpb_ObjCache_Add(aTHX_ ptr, new_sv)`, and then return the new SV.

## Advanced Cache Management

To achieve world-class performance and observability, the object cache includes (or is planned to include) the following:

-   **Scalable Lookups**: The cache is designed to maintain O(1) performance even with millions of active objects.
-   **Concurrency Integrity**: The cache implementation ensures stability during high-frequency context switching in coroutine-based environments (Coro/Mojo). Weak references are rigorously validated to ensure they remain stable during interleaved GC cycles.
-   **Eviction Policies**: (Planned) Implement LRU (Least Recently Used) or memory-pressure based clearing to ensure the cache does not exceed configurable memory bounds.
-   **Observability**: (Planned) High-performance trace/audit logging for cache hits, misses, and premature collection to aid in identifying complex memory management issues.
