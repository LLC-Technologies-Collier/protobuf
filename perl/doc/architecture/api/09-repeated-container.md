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

## Advanced Performance Goals

To achieve world-class array performance, the implementation includes (or is planned to include) the following:

-   **SIMD-Accelerated Bulk Operations**: (Planned) Implement SSE4.1/AVX2 optimized C functions for bulk appending and scanning of numeric repeated fields.
-   **COW (Copy-On-Write)**: (Planned) Support for COW semantics during repeated field cloning when messages share an arena, avoiding expensive memory copies.
-   **In-Place Search/Sort**: (Planned) High-performance C-level utilities for in-place sorting and binary searching of scalar repeated fields to support rapid data processing directly on the upb structures.
-   **Integrated Merging**: (Planned) Optimized C-level merging logic for repeated fields that can maintain deterministic ordering or perform unique-set deduplication during `parse_from` operations.
-   **Cross-Arena Deep Copy**: (Planned) Implement direct deep-copy for repeated fields when moving data between messages on different arenas, bypassing Perl-level array intermediate steps.
-   **SIMD Bulk Transfer (Perl-to-C)**: (Planned) Optimize the `push` operation for tied arrays to detect large Perl array transfers and utilize SIMD-accelerated C-level bulk population of the `upb_Array`.
-   **Native XS Operations**: (Planned) Implement high-performance `grep` and `map` logic directly in the XS layer for tied repeated fields, allowing data transformation and filtering at C-speed.
-   **Bulk Scanning Mode**: (Planned) Provide a specialized "bulk read" mode for repeated message fields that allows iterating over elements without the overhead of creating individual Perl object wrappers.

## Object Identity and Caching
...
