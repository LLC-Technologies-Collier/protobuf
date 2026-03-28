# Architecture: Memory Management

[TOC]

## C Layer (upb)

1.  **Arenas:** The `upb` library's arena allocation model will be used extensively to manage the lifetime of messages, descriptors, and related C structures.
2.  **Ownership:** C structures created and returned to Perl will typically be owned by the arena associated with the parent Perl object (e.g., a Message object's arena).

## Perl/XS Interface

1.  **Tied Lifetimes:** The lifetime of the C structures (managed by upb arenas) will be tied to the lifetime of the corresponding Perl objects.
2.  **DESTROY:** Perl objects wrapping arena-allocated structures (like `Protobuf::Arena` itself, or top-level `Protobuf::Message` objects not created within another's arena) will free the associated arena in their `DESTROY` method.
3.  **Weak References:** Care must be taken to avoid circular references between Perl objects and C structures that might prevent timely garbage collection. `SVs` stored within C structures that might point back to Perl objects should be handled with care, potentially using weakened references if necessary.
4.  **Perl-Owned Data:** Data passed from Perl to C (e.g., string values for fields) will often be copied into the upb arena. The original Perl scalar's lifetime is then decoupled.
