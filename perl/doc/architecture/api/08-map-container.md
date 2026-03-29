# Map Container

_Status: C Layer Implemented_

The `Protobuf::Map` class provides a dictionary-like interface for accessing and modifying map fields in a message.

## Implementation

The C implementation is located in `perl/xs/map/`.

*   **`map.c`**: Implements the main map operations.
    *   `PerlUpb_Map_New`: Creates a new map wrapper for a given `upb_Map`. It holds a strong reference to the arena SV to ensure the map's memory stays alive.
    *   `PerlUpb_Map_GetItem`: Retrieves the value for a key. It uses the `upb_FieldDef` of the map field to correctly convert the Perl key to a `upb_MessageValue` and the returned `upb_MessageValue` back to a Perl SV.
    *   `PerlUpb_Map_SetItem`: Sets the value for a key.
    *   `PerlUpb_Map_DeleteItem`: Removes a key from the map.
    *   `PerlUpb_Map_Size`: Returns the number of entries.
*   **`iterator.c`**: Implements an iterator for the map.
    *   Uses `upb_Map_Next` to iterate over all key/value pairs.
*   **`scalar.c` / `message.c`**: Placeholders for specialized map logic if needed. Currently, the generic implementation handles both.

## Perl Interface

In Perl, `Protobuf::Map` will provide a tied hash or overloaded interface:

```perl
my $val = $msg->map_field->{$key};
$msg->map_field->{$key} = $new_val;
delete $msg->map_field->{$key};
```

## Memory Management

*   The `Map` object holds a strong reference to the `arena_sv`.
*   The `MapIterator` holds a strong reference to the `Map` object.
*   This ensures that the underlying `upb_Map` remains valid as long as the Perl wrappers are in use.
