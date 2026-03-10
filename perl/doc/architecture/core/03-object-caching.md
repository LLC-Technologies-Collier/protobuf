# Object Caching

_Status: Not Started_

To ensure object identity and improve performance, the XS layer must cache the Perl wrappers (SVs) created for underlying UPB C objects. This will closely follow the model used in the Python UPB extension.

## Cache Mechanism

*   **Global Cache:** A single, global (per-interpreter) hash table will be used to cache all UPB-derived objects. This cache will map C pointers (e.g., `const upb_Def*`, `const upb_Message*`, `upb_Arena*`) to their corresponding Perl wrapper SVs.
*   **Keys:** String representations of the C pointer address (e.g., generated via `sprintf "%p", ptr`). Using raw integer pointer values is not reliable as hash keys.
*   **Values:** Weak references to the blessed Perl objects (e.g., `Protobuf::Descriptor`, `Protobuf::Message`).
*   **Weak References:** Using `Scalar::Util::weaken` is crucial. This prevents the cache from keeping the Perl wrapper objects alive if all other references are gone. When the Perl object is destroyed, the weak reference in the cache becomes undefined, allowing the cache entry to be eventually cleaned up.

## Cache Lookup/Insert

*   All XS functions or typemap code that convert a `upb_Def*`, `upb_Message*`, or other UPB pointer to a Perl object MUST first check the global cache.
*   If a wrapper exists for the C pointer, return the existing SV (after strengthening the reference if necessary).
*   If not found, create a new Perl wrapper object, store it in the cache with the C pointer as the key, and then return the new object.

## Important Notes on Hash Operations

*   When using string keys derived from pointers, ensure the same stringification method is used in `Add`, `Get`, and `Delete` operations.
*   `hv_delete_ent(hash, key_sv, flags, hash)` decrements the reference count of the `key_sv` provided for the lookup. Do not manually decref the `key_sv` after calling `hv_delete_ent`.
*   `hv_fetch_ent(hash, key_sv, lval, hash)` does *not* decref the `key_sv`, so it must be decreffed after the call.



## Benefits

*   **Object Identity:** `$msg->field == $msg->field` will be true, as the same underlying C object will always return the same Perl object.
*   **Performance:** Avoids repeatedly creating new Perl wrappers for the same C object.
*   **Memory:** Reduces the number of Perl objects.
*   **Simplified Management:** A single cache is easier to manage than per-object or per-arena caches.

This approach mirrors the `PyUpb_ObjCache` mechanism in the Python extension, which has proven effective.

