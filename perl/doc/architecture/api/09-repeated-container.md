# Repeated Container

_Status: Fully Implemented_

The `Protobuf::Internal::Repeated` class (exposed via generated message accessors) provides an array-like interface for accessing and modifying repeated fields in a message using Perl's `tie` mechanism.

## Implementation

### C Layer
The C implementation is located in `perl/xs/repeated/`.
...
*   **`composite.c`**: Implements specialized logic for message/group repeated fields.
    *   `PerlUpb_Repeated_Add`: Creates a new submessage on the arena, appends it to the array, and returns a Perl wrapper. Correctly uses `PerlUpb_ObjCache` for the returned object.

### Perl Layer
The Perl implementation is located in `perl/lib/Protobuf/Internal/Repeated.pm` and `perl/lib/Protobuf/Internal/Repeated.xs`.

*   Uses `Tie::Array` to provide a standard Perl array interface.
*   Maps standard array operations (`FETCH`, `STORE`, `FETCHSIZE`, `STORESIZE`, `PUSH`, `POP`, `SHIFT`, `UNSHIFT`, `CLEAR`) to C functions in `perl/xs/repeated/`.
*   Directly manipulates the underlying `upb_Array` via XS, ensuring high performance.

## Perl Interface

In Perl, repeated fields return a reference to a tied array:

```perl
my $arr = $msg->repeated_field; # Returns an ArrayRef tied to Protobuf::Internal::Repeated
push @$arr, $val;
my $val = $arr->[0];
scalar(@$arr);
@$arr = (); # CLEAR
```

## Object Identity and Caching
...
