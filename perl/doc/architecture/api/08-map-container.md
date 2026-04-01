# Map Container

_Status: Fully Implemented_

The `Protobuf::Internal::Map` class (exposed via generated message accessors) provides a dictionary-like interface for accessing and modifying map fields in a message using Perl's `tie` mechanism.

## Implementation

### C Layer
The C implementation is located in `perl/xs/map/`.
...
*   **`iterator.c`**: Implements an iterator for the map using `upb_Map_Next`.

### Perl Layer
The Perl implementation is located in `perl/lib/Protobuf/Internal/Map.pm` and `perl/lib/Protobuf/Internal/Map.xs`.

*   Uses `Tie::Hash` to provide a standard Perl hash interface.
*   Maps standard hash operations (`FETCH`, `STORE`, `DELETE`, `CLEAR`, `EXISTS`, `FIRSTKEY`, `NEXTKEY`, `SCALAR`) to C functions in `perl/xs/map/`.
*   Uses `Protobuf::Internal::MapIterator` (defined in `perl/lib/Protobuf/Internal/MapIterator.xs`) for efficient iteration.

## Perl Interface

In Perl, map fields return a reference to a tied hash:

```perl
my $map = $msg->map_field; # Returns a HashRef tied to Protobuf::Internal::Map
my $val = $map->{$key};
$map->{$key} = $new_val;
delete $map->{$key};
exists $map->{$key};
keys %$map;
```

## Memory Management
...
