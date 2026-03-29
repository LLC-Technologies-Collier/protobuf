# Message Creation

_Status: C Layer Implemented_

New Protocol Buffer message instances in Perl are created either directly or as submessages of existing messages.

## Implementation

The C implementation is located in `perl/xs/protobuf/arena.c` and exposed via `Protobuf::Arena` (Moo class).

*   **`Protobuf::Arena->new`**:
    *   Calls XS `_xs_create_raw` to allocate a `upb_Arena`.
    *   Stores the pointer in `_arena_ptr`.
*   **`DEMOLISH`**:
    *   Calls XS `_xs_destroy_raw` to free the `upb_Arena` when the Perl object is destroyed.

## Arena Ownership & Lifecycle
...

-   Top-level messages own their `upb_Arena`.
-   The Perl wrapper holds a strong reference to the `arena_sv`.
-   When the Perl message object is destroyed, its `DESTROY` method (calling `PerlUpb_Message_Free`) removes it from the cache. 
-   The `arena_sv` reference count decreases. When it reaches zero, the `upb_Arena` is freed, reclaiming all memory for the message and all its submessages.
