# Api Design

[TOC]

<!-- Section: api/01-descriptor-pool.md -->

## Descriptor Pool Management

*Status: C Layer Implemented*

The `Protobuf::DescriptorPool` class manages a collection of descriptors. It is
a wrapper around `upb_DefPool`.

### Perl Level (`Protobuf::DescriptorPool`)

-   The Perl object is a Moo class.
-   It contains an opaque pointer to the `upb_DefPool*` (via the `_pool_ptr`
    attribute).
-   It provides methods to load serialized `FileDescriptorProto` data
    (`add_serialized_file`).
-   It has methods to find message, enum, or extension definitions by their
    fully qualified name (`find_message_by_name`, etc.).

### XS Level (`DescriptorPool.xs`)

-   **`_xs_create_raw`**: Creates a new `upb_DefPool` and wraps it.
-   **`_xs_destroy_raw`**: Frees the `upb_DefPool` when the Perl object is
    destroyed.
-   **`_xs_add_serialized_file`**: Parses serialized descriptor data and adds it
    to the pool.
-   **Search methods**: Provide direct access to `upb_DefPool_Find*` functions.

### Implementation Details

-   The `upb_DefPool` owns its own memory for descriptors.
-   The object cache ensures that multiple accesses to the same descriptor
    return the same Perl wrapper.

### Performance and Robustness

-   **Lazy Loading**: (Planned) To optimize memory usage for large schema
    registries, the `DescriptorPool` will support lazy parsing of
    `FileDescriptorSet` data. Individual files within the set are only fully
    parsed into `upb_FileDef` structures when a definition within them is first
    accessed.
-   **Conflict Resolution**: (Planned) The pool implementation provides detailed
    error reporting during `add_serialized_file` operations. In the event of a
    naming conflict, the error message will include precise information about
    the conflicting files and line numbers (if available) to facilitate
    debugging in complex multi-source environments.

### Concurrency and Isolation

-   **Interpreter Isolation**: Each `PerlInterpreter` (e.g. in ithreads or
    different worker processes) normally maintains its own distinct
    `upb_DefPool` and object cache state to avoid race conditions.
-   **Global Pool Sharing**: (Planned) To optimize memory usage in large-scale
    deployments (e.g. dozens of worker processes), the architecture includes
    support for a **shared, read-only global pool**. Once a pool is "frozen" (no
    more additions allowed), it can be safely mapped into multiple interpreters,
    sharing the underlying C definitions while maintaining per-interpreter Perl
    wrappers in the object cache.
-   **Lock-Free Retrieval**: High-performance implementations should aim for
    lock-free or highly-concurrent descriptor retrieval to avoid bottlenecking
    concurrent message parsing (e.g. in Mojo or Coro).
-   **Dynamic Descriptor Reloading**: (Planned) Implement support for updating
    existing descriptor definitions in the pool without invalidating active
    message instances, supporting high-availability protocol upgrades.
-   **Reflection Cache**: (Planned) To minimize XS overhead for frequent
    reflection tasks, the `DescriptorPool` will maintain a Perl-level cache of
    message and field definitions.
-   **Symbol Dependency Visualization**: (Planned) Provide a `graph()` utility
    to export the full symbol dependency tree in DOT or JSON format for
    architectural analysis of large protocol registries.

### Lifecycle

-   The `upb_DefPool` is owned by the `Protobuf::DescriptorPool` Perl object and
    freed in `DEMOLISH`.
-   Descriptors returned from the pool hold weak or cached references; the pool
    must remain alive as long as its descriptors are in use.

--------------------------------------------------------------------------------

<!-- Section: api/02-descriptor.md -->

## Descriptor API

*Status: C Wrappers Implemented*

The `Protobuf::Descriptor` class hierarchy in Perl (e.g.,
`Protobuf::MessageDescriptor`, `Protobuf::EnumDescriptor`) provides access to
the underlying Protocol Buffer definitions using the `upb` library.

The C layer implementation (Milestone 6 & 7) provides a consistent set of
wrapper functions for all `upb_Def*` types.

### Object Identity and Lifetime

To ensure memory safety and idiomatic Perl behavior, descriptors follow these
rules:

1.  **Strict Identity**: Multiple retrievals of the same underlying `upb_Def`
    (e.g. `upb_MessageDef`) MUST return the same Perl SV. This is implemented
    via the `PerlUpb_ObjCache`.
2.  **DescriptorPool Pinning**: Every descriptor object MUST hold a **strong
    reference** to its parent `Protobuf::DescriptorPool` SV. Since `upb`
    descriptors are owned by the `upb_DefPool`, destroying the pool while a
    descriptor is still in use in Perl would lead to a use-after-free. Pinning
    ensures the C memory remains valid.
3.  **Cross-Pool Resolution**: Descriptors originating from different
    `DescriptorPool` instances are considered distinct, even if they represent
    the same proto message. Comparison logic should account for pool identity.

### C Wrapper Layer

All descriptor wrappers are located in `perl/xs/descriptor/` and follow a
consistent naming convention: `PerlUpb_<Type>Def_<Method>`.

Example: `PerlUpb_MessageDef_FullName(pTHX_ const upb_MessageDef *m)`

#### Implemented Wrappers

-   **FileDef (`file.c`):** Accessors for name, package, dependencies, and
    top-level definitions (messages, enums, extensions, services).
-   **MessageDef (`message.c`):** Accessors for name, fields (by name, number,
    or index), oneofs, nested types, and containing type.
-   **FieldDef (`field.c`):** Accessors for type, label, number, presence, and
    sub-types (message or enum).
-   **EnumDef (`enum.c`):** Accessors for name and enum values.
-   **EnumValueDef (`enum_value.c`):** Accessors for name, number, and index.
-   **OneofDef (`oneof.c`):** Accessors for name, fields, and containing type.
-   **ServiceDef (`service.c`):** Accessors for name and methods.
-   **MethodDef (`method.c`):** Accessors for name, input/output types, and
    streaming flags.

### Perl Attributes (Proposed)

-   **`_upb_def`**: Opaque pointer (stored as IV) to the C `upb` definition.
-   **`_pool`**: A strong Perl reference to the creating
    `Protobuf::DescriptorPool` to ensure its lifetime.

### Perl Methods (Proposed)

Perl methods will call the corresponding C wrappers. Methods that return other
descriptors (e.g., `field->message_type()`) MUST use the
`Protobuf::DescriptorPool` object cache to ensure Perl object identity and
efficient memory management.

### Advanced Optimization and Comparison

1.  **Lazy Descriptor Blessing**: (Planned) To optimize high-frequency
    descriptor access, the implementation will support a thread-local
    "fast-path" single-item cache. This avoids the overhead of hash lookup in
    the `ObjCache` when the same descriptor is accessed repeatedly in a tight
    loop.
2.  **Schema Fingerprinting**: (Planned) Message definitions will support stable
    hash fingerprinting. This allows for fast O(1) comparison of descriptors
    across different `DescriptorPool` instances or process boundaries, enabling
    efficient schema-registry integrations.
3.  **Thread-Safe Retrieval**: Descriptor retrieval logic is architected to be
    lock-free, ensuring that concurrent lookups in Mojo or Coro do not
    contention on global state.
4.  **Direct-to-Native Reflection**: (Planned) Implement a high-performance
    reflection path that allows internal C-layer tasks to access `upb_Def`
    information directly from the C structure, bypassing Perl object creation
    for ultra-hot loops.
5.  **Descriptor Memory Profiling**: (Planned) Provide an API to report the
    exact memory overhead for each descriptor subclass within the `upb_DefPool`,
    enabling fine-grained memory analysis of large schema sets.
6.  **Cross-Version Compatibility Layer**: (Planned) Support a descriptor-level
    mapping that allows transparent interaction between Proto2 and Proto3
    message definitions during cross-version schema evolution.

--------------------------------------------------------------------------------

<!-- Section: api/03-message-creation.md -->

## Message Creation

*Status: C Layer Implemented*

New Protocol Buffer message instances in Perl are created either directly or as
submessages of existing messages.

### Implementation

The C implementation is located in `perl/xs/protobuf/arena.c` and exposed via
`Protobuf::Arena` (Moo class).

*   **`Protobuf::Arena->new`**:
    *   Calls XS `_xs_create_raw` to allocate a `upb_Arena`.
    *   Stores the pointer in `_arena_ptr`.
*   **`DEMOLISH`**:
    *   Calls XS `_xs_destroy_raw` to free the `upb_Arena` when the Perl object
        is destroyed.

### Arena Ownership & Lifecycle

...

-   Top-level messages own their `upb_Arena`.
-   The Perl wrapper holds a strong reference to the `arena_sv`.
-   When the Perl message object is destroyed, its `DESTROY` method (calling
    `PerlUpb_Message_Free`) removes it from the cache.
-   The `arena_sv` reference count decreases. When it reaches zero, the
    `upb_Arena` is freed, reclaiming all memory for the message and all its
    submessages.

--------------------------------------------------------------------------------

<!-- Section: api/04-descriptor-pool.md -->

## Descriptor Pool

*Status: C Layer Implemented*

The `Protobuf::DescriptorPool` class is a Perl wrapper around `upb_DefPool`. It
serves as the central registry for all Protocol Buffer definitions (messages,
enums, etc.).

### Implementation

The C implementation is located in `perl/xs/descriptor_pool/`.

*   **`pool.c`**: Core management of the `upb_DefPool` wrapper.
    *   Uses the global object cache to ensure that a `upb_DefPool*` always maps
        to the same Perl object instance.
    *   Provides access to the singleton generated pool
        (`PerlUpb_DescriptorPool_GeneratedPool`).
*   **`add.c`**: Logic for adding definitions to the pool.
    *   `PerlUpb_DescriptorPool_AddSerializedFile`: Takes a serialized
        `FileDescriptorProto` string, parses it using upb's internal descriptor
        mini-tables, and adds it to the pool.
*   **`find.c`**: Logic for looking up definitions in the pool.
    *   Provides finders for files, messages, enums, and extensions by their
        full name.
    *   Returned descriptors are automatically wrapped in their corresponding
        Perl classes (e.g., `Protobuf::MessageDescriptor`) using the object
        cache.

### Object Identity and Lifecycle

The `DescriptorPool` is responsible for the lifetime of all its descriptors. In
the XS layer:

1.  The `Protobuf::DescriptorPool` Perl object holds the `upb_DefPool*`.
2.  When a descriptor is looked up, the XS layer creates (or retrieves from
    cache) a Perl wrapper (e.g., `Protobuf::MessageDescriptor`).
3.  This descriptor wrapper holds an opaque pointer to the `upb` definition but
    does *not* own it.
4.  To ensure the pool stays alive as long as any of its descriptors are in use,
    the Perl descriptor objects will hold a strong reference to the
    `DescriptorPool` object.

### Global Generated Pool

The implementation provides a singleton pool that contains all descriptors
generated by `protoc-gen-perl`. This mirrors the behavior of other Protocol
Buffer implementations (like Python's `symbol_database`).

--------------------------------------------------------------------------------

<!-- Section: api/04-message-accessors.md -->

## Message Field Accessors

*Status: Fully Implemented*

Field access in `Protobuf::Message` objects is designed to be idiomatic Perl and
highly performant.

### C Implementation

...

*   **`PerlUpb_Message_Clear`**: Clears all fields in the message.

### Perl Layer Implementation

Instead of using `AUTOLOAD`, which can be slow and less transparent, this
project uses **dynamic method injection** via `Protobuf::ClassGenerator`.

-   **Class Generation**: When a `.proto` file (or descriptor set) is loaded
    into a `Protobuf::DescriptorPool`, the `Protobuf::ClassGenerator`
    automatically creates a corresponding Perl class for every message type.
-   **Method Injection**: For each field in a message, the generator injects:
    -   A getter: `$msg->fieldname()`
    -   A setter: `$msg->set_fieldname($val)`
    -   A presence checker: `$msg->has_fieldname()`
    -   A clearer: `$msg->clear_fieldname()`
-   **Moo Integration**: The generated classes use `Moo` and inherit from
    `Protobuf::Message`.
-   **Tied Containers**: For repeated and map fields, the getters return
    references to tied arrays or hashes, providing a standard Perl collection
    interface that directly manipulates the underlying `upb` data structures.

### Serialization and Comparison

...

Implemented in `perl/xs/message/serialize.c` and `perl/xs/message/compare.c`.

*   **`PerlUpb_Message_Serialize`**: Serializes message to wire format using
    `upb_Encode`.
*   **`PerlUpb_Message_Parse`**: Creates a new message and arena, and parses
    data into it via `upb_Decode`.
*   **`PerlUpb_Message_IsEqual`**: Compares two messages for value equality
    using `upb_Message_IsEqual`.

### Advanced Performance Goals

To achieve world-class serialization and access performance, the implementation
includes (or is planned to include) the following:

-   **SIMD-Accelerated Serialization**: (Planned) Research and implement
    SSE4.1/AVX2 optimized serialization for fixed-length numeric arrays and
    large string fields.
-   **Reflection Cache**: (Planned) Implement a C-level cache for
    `upb_MiniTable` and `upb_FieldDef` lookups during dynamic field access to
    minimize reflection overhead in tight loops.
-   **COW (Copy-On-Write)**: (Planned) Architect sub-message cloning and merging
    to use COW semantics where possible when messages share an arena, avoiding
    expensive deep copies for read-only sub-graphs.
-   **Deterministic Serialization**: (Planned) Provide an option for
    deterministic field ordering during serialization to ensure stable
    wire-format signatures for caching and verification.
-   **High-Performance Merging**: (Planned) Implement optimized C-level merging
    logic (`parse_from`) that leverages arena sharing to minimize allocations
    when combining data from multiple sources.
-   **Zero-Copy Field Access**: (Planned) Implement zero-copy accessors for
    string and bytes fields that return Perl SVs pointing directly into the
    `upb` message buffer, eliminating allocation and copy overhead for large
    data blobs.
-   **Method Dispatch Bypass**: (Planned) Optimize the generated Perl classes to
    support direct XS calls for hot accessors, bypassing the Moo method dispatch
    layer in high-performance processing loops.

### Type Handling

-   The C layer uses `upb_FieldDef` to determine the field's type and uses
    `PerlUpb_SvToUpb` / `PerlUpb_UpbToSv` for conversion.
-   See [Type Conversion](core/05-type-conversion.md).

--------------------------------------------------------------------------------

<!-- Section: api/05-descriptor-containers.md -->

## Descriptor Containers

*Status: C Layer Implemented*

To efficiently expose collections of descriptors (like fields, nested messages,
enum values) from a parent descriptor, we use generic container types in XS.
This avoids creating many unique Perl classes for each type of collection.

### Implementation

The C implementation of the descriptor containers is located in
`perl/xs/descriptor_containers/`.

*   **`PerlUpb_ByNameMap` (`by_name_map.[ch]`):** A lazy Perl map wrapper for
    string-keyed upb collections. It uses a vtable to delegate operations to the
    underlying upb object.
*   **`PerlUpb_ByNumberMap` (`by_number_map.[ch]`):** A lazy Perl map wrapper
    for integer-keyed upb collections.
*   **`PerlUpb_GenericSequence` (`generic_sequence.[ch]`):** A lazy Perl
    array-like wrapper for upb collections.
*   **`PerlUpb_MapIterator` (`iterators.[ch]`):** Provides iteration support for
    the map containers.

#### VTable Pattern

Each container type uses a vtable (a struct of function pointers) to interact
with the parent upb object. This allows the same container logic to be reused
for different types of descriptors (e.g., fields of a message, values of an
enum).

Example `PerlUpb_ByNameMap_VTable`:

-   `count`: Returns the number of items.
-   `lookup`: Looks up an item by name.
-   `key`: Returns the string name for an item at a given index.
-   `value`: Returns the upb item at a given index.
-   `wrap`: An implementation-specific function that takes the raw `upb_Def*`
    and returns the appropriate blessed Perl object (using the global object
    cache).

### Perl Interface

These XS objects are blessed into internal Perl classes:

-   `Protobuf::Internals::DescriptorByNameMap`
-   `Protobuf::Internals::DescriptorByNumberMap`
-   `Protobuf::Internals::DescriptorSequence`

The Perl layer (to be implemented in Moo classes) will wrap these internal
objects and use operator overloading (e.g., `Tie::Hash`, `Tie::Array`, or
`overload`) to provide an idiomatic Perl experience.

### Object Identity and Persistence

To ensure idiomatic Perl behavior, descriptor containers follow these rules:

1.  **Cache-Aware Wrapping**: The `wrap` function in every container vtable MUST
    utilize the `PerlUpb_ObjCache`. This ensures that retrieving the same field
    or enum value multiple times, whether via name lookup, index lookup, or
    iteration, always returns the same Perl SV.
2.  **Descriptor Pinning**: Container objects MUST hold a **strong reference**
    to their parent descriptor SV (e.g., a `MessageDef`). This prevents the
    parent's `DescriptorPool` from being freed while the container is active.

### Concurrency and Stability

-   **Read-Only Multi-Access**: Containers are designed for concurrent read-only
    access. Multiple coroutines (e.g., in Coro or Mojo) can safely perform
    lookups or create iterators simultaneously.
-   **Iterator Stability**: Iterators (`PerlUpb_MapIterator`) hold a strong
    reference to their source container. This ensures that the container remains
    valid throughout the entire iteration cycle, even if the user's reference to
    the container goes out of scope.
-   **Lock-Free Progress**: The XS implementation avoids global locks for
    container operations, relying on the read-only nature of `upb_Def`
    structures to provide high-performance concurrent access.

### High-Performance Operations

1.  **Direct-to-Hash Projection**: (Implemented) To support high-frequency bulk
    access patterns, `ByNameMap_AsHash` provides C-level projection of
    containers directly into standard Perl `HV` structures. This bypasses the
    overhead of individual lazy wrapper inflation.
2.  **Engineering Excellence (Reach for More):**
    -   **Lazy Wrapper Blessing with Thread-Local Cache:** Utilize a
        per-interpreter cache to reuse descriptor wrapper objects, minimizing
        the cost of `sv_bless` during container traversal.
    -   **O(1) Reverse-Index for Enum Value Lookup:** Speed up enum
        value-to-name resolution in `ByNumberMap` using an internal C-level hash
        table.
    -   **Vectorized Batch-Lookup API:** Support bulk retrieval of descriptors
        in a single call to minimize Perl-to-XS transition overhead for complex
        schema inspections.

### Benefits

*   **Efficiency:** Avoids creating large Perl arrays/hashes upfront. C objects
    are wrapped in Perl only when they are actually accessed.
*   **Reduced Code:** Reuses the same generic container logic for many different
    descriptor collections.
*   **Memory Management:** The container objects hold a strong reference to the
    parent Perl object (e.g., the `MessageDescriptor`), ensuring the underlying
    upb definitions stay alive as long as the container is in use.

--------------------------------------------------------------------------------

<!-- Section: api/07-extension-dict.md -->

## Extension Dictionary

*Status: C Layer Implemented*

The `Protobuf::ExtensionDict` class provides a dictionary-like interface for
accessing and modifying extension fields of a message. It is used for `proto2`
messages that define extension ranges.

### Implementation

The C implementation is located in `perl/xs/extension_dict/`.

*   **`dict.c`**: Implements the main dictionary operations.
    *   `PerlUpb_ExtensionDict_New`: Creates a new dictionary wrapper for a
        given message. It holds a strong reference to the Perl message object to
        ensure the underlying `upb_Message` and its arena stay alive.
    *   `PerlUpb_ExtensionDict_GetItem`: Retrieves the value of an extension
        field using `upb_Message_GetFieldByDef`.
    *   `PerlUpb_ExtensionDict_SetItem`: Sets the value of an extension field
        using `upb_Message_SetFieldByDef`.
*   **`iterator.c`**: Implements an iterator for the extension dictionary.
    *   Uses `upb_Message_Next` to iterate over all set fields in the message
        and filters for those that are extensions.
    *   Returns `Protobuf::FieldDescriptor` objects for each set extension.

### Object Identity and Resolution

To ensure consistent behavior and high performance, extensions follow these
rules:

1.  **Lazy Extension Resolution**: The dictionary should delay the full
    inflation of extension field descriptors until they are explicitly accessed.
    This minimizes the overhead for messages with many potential extensions,
    only a few of which may be set.
2.  **Identity Persistence**: Any field descriptor returned by the dictionary
    (e.g., during iteration) MUST be verified against the `PerlUpb_ObjCache`.
    Multiple lookups of the same extension definition must return the same Perl
    SV.
3.  **Stable Mapping**: The `ExtensionDict` uses the `upb_FieldDef*` as the
    internal key. In Perl, the dictionary can be keyed by the extension field
    descriptor object.

### Memory Management and Migration

-   **Message Pinning**: The `ExtensionDict` holds a **strong reference** to the
    parent message SV.
-   **Cross-Message Migration**: Architecture supports "moving" extension data
    between messages sharing the same `upb_Arena`. This is a zero-copy operation
    at the C level, but the Perl layer must ensure that the source extension is
    cleared to prevent dual-ownership conflicts in the cache.

### Perl Interface

The `Protobuf::ExtensionDict` object is typically accessed via a message's
`extensions()` method. In Perl, it will provide a tied hash or overloaded
interface to allow idiomatic access:

```perl
my $val = $msg->extensions->{$ext_field_desc};
$msg->extensions->{$ext_field_desc} = $new_val;
```

### Memory Management

*   The `ExtensionDict` object holds a strong reference to the parent message's
    Perl SV (`message_sv`).
*   The `ExtensionDictIterator` also holds a strong reference to the parent
    message's Perl SV.
*   This ensures that the underlying `upb_Message` and its associated
    `upb_Arena` (which is also held by the message SV) are not destroyed while
    the dictionary or its iterator are in use.

--------------------------------------------------------------------------------

<!-- Section: api/08-map-container.md -->

## Map Container

*Status: Fully Implemented*

The `Protobuf::Internal::Map` class (exposed via generated message accessors)
provides a dictionary-like interface for accessing and modifying map fields in a
message using Perl's `tie` mechanism.

### Implementation

#### C Layer

The C implementation is located in `perl/xs/map/`. ...

*   **`iterator.c`**: Implements an iterator for the map using `upb_Map_Next`.

#### Perl Layer

The Perl implementation is located in `perl/lib/Protobuf/Internal/Map.pm` and
`perl/lib/Protobuf/Internal/Map.xs`.

*   Uses `Tie::Hash` to provide a standard Perl hash interface.
*   Maps standard hash operations (`FETCH`, `STORE`, `DELETE`, `CLEAR`,
    `EXISTS`, `FIRSTKEY`, `NEXTKEY`, `SCALAR`) to C functions in `perl/xs/map/`.
*   Uses `Protobuf::Internal::MapIterator` (defined in
    `perl/lib/Protobuf/Internal/MapIterator.xs`) for efficient iteration.

### Perl Interface

In Perl, map fields return a reference to a tied hash:

```perl
my $map = $msg->map_field; # Returns a HashRef tied to Protobuf::Internal::Map
my $val = $map->{$key};
$map->{$key} = $new_val;
delete $map->{$key};
exists $map->{$key};
keys %$map;
```

### Advanced Performance Goals

To achieve world-class dictionary performance, the implementation includes (or
is planned to include) the following:

-   **O(1) Bulk Projection**: (Planned) Implement a C-level function to project
    an entire `upb_Map` into a native Perl hash in a single XS call, eliminating
    the overhead of repeated `FETCH` operations for bulk reads.
-   **Shared-Arena Key Deduplication**: (Planned) Enhance the arena allocator to
    deduplicate string keys across multiple maps, significantly reducing memory
    footprint for large message sets with common keys.
-   **Collision Analysis**: (Planned) Provide diagnostic utilities to monitor
    and report hash collision rates within `upb_Map` instances to aid in schema
    and performance optimization.

### Memory Management

...

--------------------------------------------------------------------------------

<!-- Section: api/09-repeated-container.md -->

## Repeated Container

*Status: Fully Implemented*

The `Protobuf::Internal::Repeated` class (exposed via generated message
accessors) provides an array-like interface for accessing and modifying repeated
fields in a message using Perl's `tie` mechanism.

### Implementation

#### C Layer

The C implementation is located in `perl/xs/repeated/`. ...

*   **`composite.c`**: Implements specialized logic for message/group repeated
    fields.
    *   `PerlUpb_Repeated_Add`: Creates a new submessage on the arena, appends
        it to the array, and returns a Perl wrapper. Correctly uses
        `PerlUpb_ObjCache` for the returned object.

#### Perl Layer

The Perl implementation is located in `perl/lib/Protobuf/Internal/Repeated.pm`
and `perl/lib/Protobuf/Internal/Repeated.xs`.

*   Uses `Tie::Array` to provide a standard Perl array interface.
*   Maps standard array operations (`FETCH`, `STORE`, `FETCHSIZE`, `STORESIZE`,
    `PUSH`, `POP`, `SHIFT`, `UNSHIFT`, `CLEAR`) to C functions in
    `perl/xs/repeated/`.
*   Directly manipulates the underlying `upb_Array` via XS, ensuring high
    performance.

### Perl Interface

In Perl, repeated fields return a reference to a tied array:

```perl
my $arr = $msg->repeated_field; # Returns an ArrayRef tied to Protobuf::Internal::Repeated
push @$arr, $val;
my $val = $arr->[0];
scalar(@$arr);
@$arr = (); # CLEAR
```

### Advanced Performance Goals

To achieve world-class array performance, the implementation includes (or is
planned to include) the following:

-   **SIMD-Accelerated Bulk Operations**: (Planned) Implement SSE4.1/AVX2
    optimized C functions for bulk appending and scanning of numeric repeated
    fields.
-   **COW (Copy-On-Write)**: (Planned) Support for COW semantics during repeated
    field cloning when messages share an arena, avoiding expensive memory
    copies.
-   **In-Place Search/Sort**: (Planned) High-performance C-level utilities for
    in-place sorting and binary searching of scalar repeated fields to support
    rapid data processing directly on the upb structures.
-   **Integrated Merging**: (Planned) Optimized C-level merging logic for
    repeated fields that can maintain deterministic ordering or perform
    unique-set deduplication during `parse_from` operations.
-   **Cross-Arena Deep Copy**: (Planned) Implement direct deep-copy for repeated
    fields when moving data between messages on different arenas, bypassing
    Perl-level array intermediate steps.
-   **SIMD Bulk Transfer (Perl-to-C)**: (Planned) Optimize the `push` operation
    for tied arrays to detect large Perl array transfers and utilize
    SIMD-accelerated C-level bulk population of the `upb_Array`.
-   **Native XS Operations**: (Planned) Implement high-performance `grep` and
    `map` logic directly in the XS layer for tied repeated fields, allowing data
    transformation and filtering at C-speed.
-   **Bulk Scanning Mode**: (Planned) Provide a specialized "bulk read" mode for
    repeated message fields that allows iterating over elements without the
    overhead of creating individual Perl object wrappers.

### Object Identity and Caching

...

--------------------------------------------------------------------------------

<!-- Section: api/10-unknown-fields.md -->

## Unknown Fields

*Status: Fully Implemented*

Unknown fields are fields encountered during parsing that are not recognized by
the message definition. UPB preserves these fields in the message's internal
buffer.

### Implementation

#### C Layer

The C implementation is located in `perl/xs/unknown_fields/`. ... *
`PerlUpb_UnknownFieldSet_Clear`: Discards all unknown fields using
`_upb_Message_DiscardUnknown_shallow`.

#### Perl Layer

The Perl implementation is located in `perl/lib/Protobuf/UnknownFieldSet.pm` and
`perl/lib/Protobuf/UnknownFieldSet.xs`.

### Perl Interface

...

In Perl, `Protobuf::UnknownFieldSet` will provide access to the raw data:

```perl
my $unknown_data = $msg->unknown_fields->data;
$msg->unknown_fields->add($raw_bytes);
$msg->unknown_fields->clear;
```

### Advanced Features and Performance

To achieve world-class observability and flexibility, the implementation
includes (or is planned to include) the following:

-   **Direct Unknown-to-Message Conversion**: (Planned) Implement C-level logic
    to attempt parsing existing unknown field blobs into a newly provided
    `MessageDef`. This facilitates "lazy schema upgrades" where previously
    unrecognized data can be reified into typed fields.
-   **Unknown Field Indexing**: (Planned) For high-frequency tag analysis,
    implement internal C-level indexing of the unknown buffer to allow O(1)
    existence checks and retrieval of specific unknown tags.
-   **Trace Auditing**: (Planned) Integrate with the core trace/audit log to
    record discovery of unknown fields, aiding in identifying version drift in
    distributed systems.

### Memory Management

*   The `UnknownFieldSet` object holds a strong reference to the parent
    message's Perl SV.
*   This ensures the underlying `upb_Message` and its arena stay alive.

--------------------------------------------------------------------------------

<!-- Section: api/11-well-known-types.md -->

## Well-Known Types (WKT)

*Status: Fully Implemented*

The Protocol Buffers Well-Known Types (WKT) provide a standard set of messages
for common patterns such as timestamps, durations, and dynamic data (`Any`,
`Struct`).

### Implementation

#### Dynamic and Wrapper Types

-   **`google::protobuf::Any`**: Implements `pack($msg)` and `unpack()` methods.
    Unpacking uses the `DescriptorPool` to reify the serialized blob.
-   **`google::protobuf::Struct`**: Provides a Perl-to-Message bridge
    (`from_perl`, `to_perl`) to convert between native Perl nested data
    structures (HashRefs/ArrayRefs) and the `Struct` / `ListValue` protocol
    types.
-   **Wrappers**: Types like `StringValue`, `Int32Value`, etc., are handled by
    the standard class generation and provide a nullable alternative to scalar
    fields.

#### Temporal Types

-   **`google::protobuf::Timestamp`**: Supports conversion to/from POSIX epoch
    and `Time::Piece` objects.
-   **`google::protobuf::Duration`**: Supports conversion to/from floating-point
    seconds.

### Advanced Performance Goals

To achieve world-class integration for common data patterns, the implementation
includes (or is planned to include) the following:

-   **Direct Any Unpacking (C-Layer)**: (Planned) Implement a high-performance
    integration that can unpack an `Any` message directly into a C structure
    without intermediate Perl object creation, using a cached `upb_DefPool`.
-   **Vectorized Temporal Conversions**: (Planned) Implement SSE4.1/AVX2
    optimized conversion logic for `Timestamp` and `Duration` types to support
    high-throughput log processing and event analysis.
-   **WKT-Specific Memory Profiling**: (Planned) Provide detailed memory
    overhead reports for complex `Struct` and `ListValue` objects, aiding in the
    identification of memory bloat in dynamic protocols.

### Memory Management

-   Well-Known Types are standard `Protobuf::Message` objects and follow the
    same arena-based lifecycle rules.
-   `Any->unpack()` creates a new message instance that shares the parent `Any`
    object's arena unless otherwise specified, ensuring efficient data
    migration.

--------------------------------------------------------------------------------

<!-- Section: api/12-text-format.md -->

## TextFormat

*Status: Fully Implemented*

The `TextFormat` implementation in Perl provides a human-readable representation
of Protocol Buffer messages, primarily for debugging and configuration.

### Implementation

The implementation is integrated into `Protobuf::Message`.

-   **`to_text()`**: Calls the C-layer `upb_TextEncode` function to generate a
    string representation of the message.
-   **Parsing**: (Planned) Support for parsing TextFormat data into message
    instances using `upb_TextDecode`.

### Advanced Performance Goals

To achieve world-class observability and debugging support, the implementation
includes (or is planned to include) the following:

-   **Direct-to-Stream Printing**: (Planned) Implement a high-performance
    C-level path that can print text format data directly to a Perl file handle
    or a raw file descriptor, avoiding intermediate string allocations.
-   **Lossless Unknown Field Formatting**: (Planned) Enhance the text output to
    include structured, human-readable representations of unrecognized fields to
    facilitate debugging of schema mismatches.
-   **Custom Redactors**: (Planned) Support for registering redaction filters to
    automatically mask sensitive field data (e.g., PII) during text formatting
    for logging purposes.

### Memory Management

-   TextFormat generation currently allocates a temporary buffer in the C layer,
    which is then copied into a Perl SV.
-   Direct-to-stream printing will eliminate this intermediate copy.

--------------------------------------------------------------------------------

<!-- Section: api/13-json-format.md -->

## JSON Format

*Status: Fully Implemented*

The JSON implementation in Perl provides standard-compliant Protocol Buffer JSON
serialization and deserialization using the `upb` library.

### Implementation

The implementation is integrated into `Protobuf::Message`.

-   **`to_json()`**: Calls the C-layer `upb_JsonEncode` function to generate a
    JSON representation of the message.
-   **`from_json()`**: Calls the C-layer `upb_JsonDecode` function to parse JSON
    into a message instance.

### Advanced Performance Goals

To achieve world-class high-throughput JSON processing, the implementation
includes (or is planned to include) the following:

-   **Direct JSON-to-Wire Conversion**: (Planned) Implement a high-performance
    C-level integration that can parse JSON data directly into a wire-format
    serialized blob, bypassing intermediate Perl objects.
-   **Streaming JSON Serialization**: (Planned) Implement a C-level JSON
    generator that can stream message data directly to a Perl file handle or
    socket, significantly reducing memory overhead for large responses.
-   **JSON Schema Mapping Verification**: (Planned) Provide a utility to verify
    that the JSON representation of a message remains consistent with its
    JSON-schema definition during schema evolution.

### Memory Management

-   JSON generation currently allocates a temporary buffer in the C layer, which
    is then copied into a Perl SV.
-   Direct-to-stream printing will eliminate this intermediate copy.

--------------------------------------------------------------------------------

<!-- Section: api/14-protoc-plugin.md -->

## protoc Plugin Architecture

*Status: Planned*

The `protoc-gen-perl-pb` plugin is responsible for generating Perl modules from
`.proto` files.

### Implementation

The plugin follows the standard `protoc` plugin protocol:

-   Reads a `CodeGeneratorRequest` from STDIN.
-   Processes the `FileDescriptorProto` objects.
-   Writes a `CodeGeneratorResponse` to STDOUT.

#### Core Logic

-   **Module Generation**: Creates a `.pm` file for each Protobuf package or
    file.
-   **Dynamic Initialization**: Generates code that loads the serialized
    descriptor set into the `Protobuf::DescriptorPool` and triggers the creation
    of Perl classes via `Protobuf::ClassGenerator`.
-   **Moo Integration**: The generated modules utilize `Moo` for inheritance and
    attribute management.

### Advanced Generation Goals

To achieve world-class developer ergonomics and performance, the plugin includes
(or is planned to include) the following:

-   **C-Layer Code Generation**: (Planned) Implement the plugin using a
    high-performance C-layer generator that utilizes `upb` descriptors directly,
    ensuring 100% logic alignment with the runtime library.
-   **Integrated Async Services**: (Planned) Automatically generate asynchronous
    service stubs for `Mojo::IOLoop` and `Coro` when services are defined in the
    `.proto` file.
-   **Embedded Schema Metadata**: (Planned) Embed full serialized descriptor
    data within each generated module to allow for "offline" reflection and
    self-describing modules.
-   **Type::Tiny Library Generation**: (Planned) Automatically generate
    corresponding `Type::Tiny` type libraries for each package to support
    rigorous business logic validation.

### Build Integration

-   The plugin will be distributable as a standalone executable.
-   Support for `Module::Build` and `ExtUtils::MakeMaker` integration to
    automate code generation during the Perl build process.

--------------------------------------------------------------------------------
