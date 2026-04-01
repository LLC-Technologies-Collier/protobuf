# Message Field Accessors

_Status: Fully Implemented_

Field access in `Protobuf::Message` objects is designed to be idiomatic Perl and highly performant.

## C Implementation
...
*   **`PerlUpb_Message_Clear`**: Clears all fields in the message.

## Perl Layer Implementation

Instead of using `AUTOLOAD`, which can be slow and less transparent, this project uses **dynamic method injection** via `Protobuf::ClassGenerator`.

-   **Class Generation**: When a `.proto` file (or descriptor set) is loaded into a `Protobuf::DescriptorPool`, the `Protobuf::ClassGenerator` automatically creates a corresponding Perl class for every message type.
-   **Method Injection**: For each field in a message, the generator injects:
    -   A getter: `$msg->fieldname()`
    -   A setter: `$msg->set_fieldname($val)`
    -   A presence checker: `$msg->has_fieldname()`
    -   A clearer: `$msg->clear_fieldname()`
-   **Moo Integration**: The generated classes use `Moo` and inherit from `Protobuf::Message`.
-   **Tied Containers**: For repeated and map fields, the getters return references to tied arrays or hashes, providing a standard Perl collection interface that directly manipulates the underlying `upb` data structures.

## Serialization and Comparison
...

Implemented in `perl/xs/message/serialize.c` and `perl/xs/message/compare.c`.

*   **`PerlUpb_Message_Serialize`**: Serializes message to wire format using `upb_Encode`.
*   **`PerlUpb_Message_Parse`**: Creates a new message and arena, and parses data into it via `upb_Decode`.
*   **`PerlUpb_Message_IsEqual`**: Compares two messages for value equality using `upb_Message_IsEqual`.

## Type Handling

-   The C layer uses `upb_FieldDef` to determine the field's type and uses `PerlUpb_SvToUpb` / `PerlUpb_UpbToSv` for conversion.
-   See [Type Conversion](core/05-type-conversion.md).
