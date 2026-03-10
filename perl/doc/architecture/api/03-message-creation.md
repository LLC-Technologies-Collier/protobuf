# Message Creation

_Status: Not Started_

New Protocol Buffer message instances in Perl are created either directly or as submessages of existing messages.

## Methods

1.  **`Protobuf::Message->new(_descriptor => $descriptor, ...)`:**
    -   Creates a *top-level* message.
    -   Requires a `Protobuf::Descriptor` object.
    -   A new `upb_Arena` is created and owned by this `Protobuf::Message` instance (via lazy builder `_build_upb_arena`).
    -   The `upb_MessageDef*` is retrieved from the `_descriptor` object.
    -   The `upb_MiniTable*` is retrieved from the `upb_MessageDef` using `upb_MessageDef_MiniTable()`.
    -   A new `upb_Message` is created on the message's own arena using `upb_Message_New()`.
    -   The Perl `Protobuf::Message` object stores pointers to the `upb_Message`, the `upb_Arena`, and the `Protobuf::Descriptor`.

2.  **Accessing a singular message field (e.g., `$parent->submessage`):**
    -   If the field is not set, this returns a "stub" `Protobuf::Message` object. This stub does not yet have its own `upb_Message` or `upb_Arena`. It contains:
        *   A reference to the parent `Protobuf::Message` SV.
        *   The `upb_FieldDef*` for the submessage field.
    -   When a field on the stub is *mutated*, the stub is "reified":
        *   The parent's arena is used.
        *   `upb_Message_Mutable()` is called on the parent to create the sub-`upb_Message` instance.
        *   The stub Perl object is updated to point to this new `upb_Message` and the parent's arena.

3.  **`$pool->new_message('My.Message.Name', ?%attrs)`:** (Planned) This factory method on the pool is a more user-friendly way to create top-level messages.

## XS Layer Responsibilities (`Message.xs`)

-   `pb_msg_create_arena()`: Creates a new `upb_Arena` for a top-level message instance.
-   `pb_msg_free_arena()`: Frees the message's arena in `DEMOLISH`.
-   XSUBs for field access (called by `AUTOLOAD`):
    -   Detect if the target object is a stub.
    -   If writing to a stub, reify it using the parent's arena and `upb_Message_Mutable()`.
    -   Perform the get/set/has/clear operation using the appropriate `upb/message/accessors.h` function.

## Arena Ownership & Lifecycle

-   Top-level messages own their arena.
-   Submessages (once reified) live on their parent's arena.
-   Stubs do not own an arena.
