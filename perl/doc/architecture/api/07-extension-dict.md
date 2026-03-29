# Extension Dictionary

_Status: C Layer Implemented_

The `Protobuf::ExtensionDict` class provides a dictionary-like interface for accessing and modifying extension fields of a message. It is used for `proto2` messages that define extension ranges.

## Implementation

The C implementation is located in `perl/xs/extension_dict/`.

*   **`dict.c`**: Implements the main dictionary operations.
    *   `PerlUpb_ExtensionDict_New`: Creates a new dictionary wrapper for a given message. It holds a strong reference to the Perl message object to ensure the underlying `upb_Message` and its arena stay alive.
    *   `PerlUpb_ExtensionDict_GetItem`: Retrieves the value of an extension field using `upb_Message_GetFieldByDef`.
    *   `PerlUpb_ExtensionDict_SetItem`: Sets the value of an extension field using `upb_Message_SetFieldByDef`.
*   **`iterator.c`**: Implements an iterator for the extension dictionary.
    *   Uses `upb_Message_Next` to iterate over all set fields in the message and filters for those that are extensions.
    *   Returns `Protobuf::FieldDescriptor` objects for each set extension.

## Perl Interface

The `Protobuf::ExtensionDict` object is typically accessed via a message's `extensions()` method. In Perl, it will provide a tied hash or overloaded interface to allow idiomatic access:

```perl
my $val = $msg->extensions->{$ext_field_desc};
$msg->extensions->{$ext_field_desc} = $new_val;
```

## Memory Management

*   The `ExtensionDict` object holds a strong reference to the parent message's Perl SV (`message_sv`).
*   The `ExtensionDictIterator` also holds a strong reference to the parent message's Perl SV.
*   This ensures that the underlying `upb_Message` and its associated `upb_Arena` (which is also held by the message SV) are not destroyed while the dictionary or its iterator are in use.
