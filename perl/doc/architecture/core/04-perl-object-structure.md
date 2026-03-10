# Perl Object Structure

Perl objects wrapping Protobuf messages will be designed to be lightweight and delegate most operations to the XS layer, which interfaces with the `upb` library.

## Message Object (`Protobuf::Message`)

A Perl object of a specific message type (e.g., `MyNamespace::MyMessage`) will be a blessed hash. It can be in one of two states:

1.  **Reified:** Contains the full `upb_Message`.
    *   **`_upb_message`**: An opaque pointer (stored as an integer) to the `upb_Message` struct in C.
    *   **`_upb_arena`**: An opaque pointer to the `upb_Arena` owned by *this message instance* (if top-level) or the parent's arena if a submessage.
    *   **`_descriptor`**: A reference to the `Protobuf::Descriptor` object.
    *   **`_parent`**: Weak reference to the parent `Protobuf::Message` if this is a submessage.

2.  **Stub (for unset singular message fields):** Represents a potential message that hasn't been allocated yet.
    *   **`_parent`**: A strong reference to the parent `Protobuf::Message`.
    *   **`_field_descriptor`**: A reference to the `Protobuf::Descriptor` for the *field* this stub represents.
    *   No `_upb_message` or `_upb_arena` of its own. It will use the parent's arena when reified.

## Descriptor Object (`Protobuf::Descriptor`)

1.  **`_upb_def`**: Opaque pointer to the C definition (`const upb_MessageDef*`, etc.).
2.  **`_type`**: String indicating the definition type (e.g., 'message').
3.  **`_pool`**: A strong Perl reference to the `Protobuf::DescriptorPool` instance that created and owns this definition, ensuring the C memory remains valid.

## Field Access

-   **`AUTOLOAD`:** Field accessors (getters, setters, has, clear) are primarily handled by `AUTOLOAD` in `Protobuf::Message`.
-   **Dispatch to XS:** `AUTOLOAD` determines the operation and field, calling XS functions.
-   **XS Operations:**
    -   If the message is a stub, any write operation will reify it, creating the `upb_Message` on the parent's arena.
    -   XS functions use the `upb_FieldDef` (from the `Protobuf::Descriptor`) and the `upb_Message` pointer to call the appropriate `upb/message/accessors.h` functions.

## Example Stub Reification Flow

1.  `$parent->submessage` is called. XS returns a "stub" `Protobuf::Message` blessed hash containing `_parent` and `_field_descriptor`.
2.  `$stub->field(123)` is called. The setter in XS detects it's a stub, retrieves the parent's `upb_Message` and `upb_Arena`, creates the submessage using `upb_Message_Mutable`, and updates the stub SV in place to become a reified message SV, populating `_upb_message`, `_upb_arena`, and `_descriptor`.
3.  The field value is set on the now reified `upb_Message`.

This "copy-on-write" like behavior for submessages minimizes arena allocations until fields are actually used.
