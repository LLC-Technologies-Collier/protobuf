# Perl Object Structure

Perl objects wrapping Protobuf messages are designed to be lightweight and delegate most operations to the XS layer, which interfaces with the `upb` library.

## Message Object (`Protobuf::Message`)

A Perl object of a specific message type (e.g., `test::TestMessage`) is a blessed hash.

*   **`upb_msg`**: An opaque pointer (stored as an IV) to the `upb_Message` struct in C.
*   **`arena_sv`**: A reference to the `Protobuf::Arena` (or the parent's `arena_sv`) that owns the memory for this message. This keeps the arena alive as long as any message or sub-message referencing it exists.
*   **`_descriptor`**: An opaque pointer to the `upb_MessageDef` (C definition).

### Sub-Message Behavior
Unlike some implementations that use "Stub" objects, this implementation returns **`undef`** for unset singular message fields. Accessing a sub-message getter does not automatically instantiate it unless it already exists on the wire or has been explicitly set.

## Descriptor Objects (`Protobuf::Descriptor::*`)

Descriptor objects (e.g., `Protobuf::Descriptor::MessageDef`, `Protobuf::Descriptor::Field`) follow a similar structure:
1.  **`_ptr`**: Opaque pointer to the C definition (e.g., `const upb_MessageDef*`).
2.  **`_pool`**: A reference to the `Protobuf::DescriptorPool` instance that owns the definition, ensuring the C memory remains valid.

## Field Access

Instead of using `AUTOLOAD`, which can be slow and less transparent, this project uses **dynamic method injection** via `Protobuf::ClassGenerator`.

-   **Method Injection**: When a descriptor is loaded, `Protobuf::ClassGenerator` injects getters (`fieldname`), setters (`set_fieldname`), presence checkers (`has_fieldname`), and clearers (`clear_fieldname`) directly into the generated message class.
-   **Dispatch to XS**: These generated methods call lightweight wrapper methods (`get`, `set`, etc.) in `Protobuf::Message`, which then dispatch to the XS layer.
-   **XS Operations**: The XS layer uses the stored `upb_msg` and the field's `upb_FieldDef` to interact with the underlying `upb` data.
