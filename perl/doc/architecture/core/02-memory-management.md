# Memory Management

The Perl Protobuf implementation, like its Ruby and PHP counterparts, will heavily rely on `upb`'s arena-based memory management.

## Key Concepts

1.  **`upb_Arena` Ownership for Messages:** Each top-level Perl `Protobuf::Message` object instance will own a `upb_Arena`. This arena is created when the message object is instantiated (via lazy builder) and freed in `DEMOLISH`.
2.  **`upb_Arena` Ownership for Pools:** Each `Protobuf::DescriptorPool` instance also owns a `upb_Arena`. This arena is used for parsing `FileDescriptorSet` messages and for the storage of the C `upb_DefPool` and all definitions it loads. It's created via a lazy builder and freed in `DEMOLISH`.
3.  **Lifecycle Management:**
    *   The `Protobuf::Message` arena's lifecycle is tied to the Perl message object.
    *   The `Protobuf::DescriptorPool` arena's lifecycle is tied to the pool object.
    *   **Crucial:** `Protobuf::Descriptor` objects, which contain raw pointers to C definitions (like `upb_MessageDef*`), *must* hold a strong reference to the `Protobuf::DescriptorPool` instance that owns the underlying C `upb_DefPool`. This prevents the pool from being destroyed prematurely.
4.  **Submessages & Arenas:** When a message field is accessed, the returned `Protobuf::Message` object (whether a new stub or a reified message) uses the *parent's* arena. All upb C functions for field access (`upb_Message_Set...`, `upb_Message_Get...`, `upb_Message_Mutable`) handle the arena management internally. There is no need for explicit arena fusing in the XS layer.
5.  **Object Caching:** To ensure object identity and efficiency, the XS layer should maintain caches (e.g., hash tables) mapping UPB C pointers to their corresponding Perl wrapper SVs. This applies to:
    *   `upb_MessageDef*`, `upb_EnumDef*`, etc. -> `Protobuf::Descriptor` SVs (cached within the pool).
    *   `upb_Message*` -> `Protobuf::Message` SVs (cached on the arena or parent message).
    Functions returning wrappers must consult and update these caches.
6.  **Message Stubs for Singular Fields:** An unset singular message field will not initially have a `upb_Message` allocated. Accessing such a field will return a lightweight "stub" `Protobuf::Message` Perl object. This stub holds a reference to the parent message and the `upb_FieldDef*`. Only when a field within the stub is *modified* will the `upb_Message` be allocated on the parent's arena, and the stub becomes fully "reified".

## Perl Implementation

-   `Protobuf::Message` and `Protobuf::DescriptorPool` manage their respective `upb_Arena*` as integer attributes in the Perl hash, handled by XS builder and `DEMOLISH` methods.
-   `Protobuf::Descriptor` holds a `_pool` attribute which is a reference to the creating `Protobuf::DescriptorPool`.
-   XS functions for creating/accessing descriptors and messages will use hash tables to cache and retrieve Perl wrappers for C pointers.
-   The `xs/types.c` conversion layer will ensure any data copied from Perl (like strings) is allocated on the correct `upb_Arena` provided to the conversion functions.
