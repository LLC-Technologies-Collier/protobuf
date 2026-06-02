# Object Caching

To ensure object identity and performance, we will implement a caching mechanism similar to the Python UPB extension. This cache will map underlying C pointers (e.g., `upb_MessageDef*`, `upb_Message*`, `upb_Arena*`) to their corresponding Perl wrapper objects.

## Cache Implementation

The cache will likely be implemented as a Perl hash. The keys will be the memory addresses of the C pointers (cast to integers), and the values will be weak references to the Perl objects.

-   **Weak References:** Using `WeakRef` is crucial to prevent the cache from keeping objects alive longer than necessary. When the last non-weak reference to a Perl object is gone, the object will be destroyed, and the weak reference in the cache will become undefined.

-   **Scope:** The cache could be global, or more appropriately, tied to a `Protobuf::Arena` instance. Since `upb` objects are arena-allocated, a per-arena cache makes sense. Objects from different arenas should not be mixed.

## Cache Usage

When a C function returns a `upb` pointer that needs to be exposed to Perl:

1.  Check if the pointer exists as a key in the cache.
2.  If found, return the cached Perl object.
3.  If not found, create a new Perl wrapper object, store it in the cache with the C pointer as the key, and then return the new object.

This applies to:

-   Descriptors (`upb_MessageDef*`, `upb_FieldDef*`, etc.)
-   Messages (`upb_Message*`)
-   Arenas (`upb_Arena*`)

This strategy ensures that multiple requests for the same underlying C object always return the same Perl object, maintaining referential integrity.
