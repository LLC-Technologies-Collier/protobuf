# Message Creation

_Status: C Layer Implemented_

New Protocol Buffer message instances in Perl are created either directly or as submessages of existing messages.

## Implementation

The C implementation is located in `perl/xs/message/message.c`.

*   **`PerlUpb_Message_NewMessage`**: Creates a new message instance.
    *   Allocates a new `upb_Arena` via `PerlUpb_Arena_New`.
    *   Creates a new `upb_Message` using `upb_Message_New` and the message's `upb_MiniTable`.
    *   Wraps them in a Perl hash SV (blessed into the message's full name).
    *   Registers the message in the `PerlUpb_ObjCache` to ensure object identity and proper reference counting.

## Methods (Perl Perspective)

1.  **`Protobuf::Message->new()`**: (Future Perl layer) will call `PerlUpb_Message_NewMessage`.
2.  **Accessing a singular message field**: 
    -   Handled by `PerlUpb_Message_GetField`.
    -   If the field is a message type, it returns a Perl wrapper around the existing `upb_Message` pointer.
    -   Uses `PerlUpb_ObjCache` to ensure that multiple accesses to the same submessage return the same Perl SV.

## Arena Ownership & Lifecycle

-   Top-level messages own their `upb_Arena`.
-   The Perl wrapper holds a strong reference to the `arena_sv`.
-   When the Perl message object is destroyed, its `DESTROY` method (calling `PerlUpb_Message_Free`) removes it from the cache. 
-   The `arena_sv` reference count decreases. When it reaches zero, the `upb_Arena` is freed, reclaiming all memory for the message and all its submessages.
