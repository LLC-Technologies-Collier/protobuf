# Interfacing with UPB from Perl XS

This document details how the Perl XS layer should interact with the `upb` C library, drawing on patterns observed in the PHP and Python `upb` extensions.

[TOC]

## 1. Core Principles

*   **Arenas are Fundamental:** All allocations of `upb` objects (messages, arrays, maps) and their data (strings, bytes) MUST happen within a `upb_Arena`.
*   **Ownership:** The Perl wrapper objects (`Protobuf::Message`, `Protobuf::DescriptorPool`, etc.) OWN their primary `upb_Arena` instances. Destruction of the Perl object (in `DEMOLISH`) triggers the freeing of the associated arena.
*   **Lifecycle Management:** References between Perl objects are crucial. `Protobuf::Descriptor` MUST hold a reference to its `Protobuf::DescriptorPool`. Stub messages MUST hold a reference to their parent message.
*   **MiniTables:** `upb` operations on messages heavily rely on `upb_MiniTable` structures.
*   **Error Handling:** Check return values and `upb_Status` from `upb` functions and propagate errors to Perl using `croak`.
*   **Object Caching:** To ensure that the same C pointer (e.g., `upb_MessageDef*`, `upb_Message*`) always yields the same Perl object (SV), a cache (hash table) should be used. This is essential for object identity and performance.
*   **Message Stubs:** Singular message fields that are not set should not cause a `upb_Message` allocation. Instead, a lightweight "stub" Perl object should be returned, delaying allocation until the field is modified.

## 2. Mirroring the Python Protocol Buffers Implementation

To maximize code reuse and benefit from the design choices made in the mature Python UPB implementation, the Perl Protocol Buffers implementation should closely mirror its structure, both at the C-level and the Perl-level.

*   **C File Organization (`perl/xs/`):** The C files in `perl/xs/` are organized into subdirectories (e.g., `perl/xs/message/`, `perl/xs/descriptor/`) corresponding to the major components, mirroring the layout of the `python/` directory in the main protobuf project. Within these subdirectories, files are further broken down (e.g., `perl/xs/descriptor/message.c`, `perl/xs/descriptor/enum.c`). This layer directly interfaces with the `libupb` libraries. See [XS File Organization](07-xs-file-organization.md).

*   **Perl Module Organization (`perl/lib/Protobuf/`):** The Perl module files should be structured to mirror the Python modules in `python/google/protobuf/`. Core classes like `Message`, `Descriptor`, `DescriptorPool` in Python will have corresponding `Protobuf::Message`, `Protobuf::Descriptor`, `Protobuf::DescriptorPool` modules in Perl.

*   **Naming Conventions:**
    *   **Perl Modules:** Use the `Protobuf::` namespace for user-facing classes.
    *   **C Functions/Types:** Internal C types and functions within the XS layer should be named analogously to their Python C API counterparts (e.g., functions in `perl/xs/descriptor.c` will be similar to those in `python/descriptor.c`).

*   **Adaptation:** While the core logic and structure will be mirrored, a straight port is not possible.
    *   **C Layer:** The C code in `perl/xs/` will use Perl's API (e.g., `SV*`, `HV*`, `newSViv`, `hv_store`) instead of Python's C API (PyObject*, PyDict_, etc.).
    *   **Perl Layer:** The `.pm` files will use Perl idioms (Moo, XS, etc.) to provide the high-level API, while the underlying implementation in C will be structurally similar to the Python version's C code.

This approach aims to reduce the risk of design flaws, enhance maintainability by providing a consistent structure across language implementations, and benefit from the existing work done for the Python bindings.

## 4. Coro Safety Considerations

When writing XS code that interacts with the Perl interpreter, special care must be taken to ensure Coro safety. Since Coro works by saving and switching Perl interpreter contexts, any C code that can be called from Perl must be re-entrant and avoid unsafe access to shared C global data.

*   **Avoid C Globals:** Do not use static or global variables in C to store state that is not constant. If state must be shared, use Perl's features or thread-local storage if appropriate (though standard C tls is not aware of Coro threads).
*   **Perl API:** Be mindful of functions that interact with the Perl stack or interpreter state. Generally, standard XS macros and functions are safe if used correctly.
*   **Re-entrancy:** Ensure that C functions can be called again while a previous invocation on the same data structures is in progress in a different Coro thread. This is particularly important for functions that modify shared state, like the object cache.
*   **UPB and Arenas:** UPB's arena-based allocation is generally Coro-safe, as arenas are typically confined to a single message tree or pool. Problems arise if C code outside of UPB introduces shared mutable state.


### 2.1. Arena Management

*   **Creation:** `upb_Arena_New()`: Called in the XS builders for top-level `Protobuf::Message` and `Protobuf::DescriptorPool`.
*   **Freeing:** `upb_Arena_Free()`: Called in `DEMOLISH`.
*   **Fusing:** Arena fusing is handled *internally* by UPB's message accessor functions. When you set a submessage, UPB takes care of any necessary memory management on the parent's arena.
*   **Implementation:** The C functions for wrapping and managing `upb_Arena` (e.g., `PerlUpb_Arena_New`, `PerlUpb_Arena_Get`) are located in `xs/protobuf.c`.

### 2.2. Descriptor Pool Operations (`upb_DefPool`)

*   **Loading Descriptors:** `upb_DefPool_AddFile()`: Used in `DescriptorPool.xs`.
*   **Finding Definitions:** `upb_DefPool_FindMessageByName()`, etc. The returned C pointers should be used as keys to the object cache to get/create the `Protobuf::Descriptor` Perl wrapper.

### 2.3. Message Operations (`upb_Message`)

*   **Creation:**
    *   Top-level: `upb_Message_New()` on the message's own new arena.
    *   Submessage (when reifying a stub): `upb_Message_Mutable()` on the *parent's* arena.
*   **Field Access:** Use functions from `upb/message/accessors.h`. These functions handle arena operations for strings and submessages.
    *   `upb_Message_Get...`: Retrieve field values. For message fields, this might return a C pointer that needs to be wrapped in a Perl stub if not already cached.
    *   `upb_Message_Set...`: Set field values.
    *   `upb_Message_Has...`: Check field presence.
    *   `upb_Message_Clear...`: Clear fields.
    *   `upb_Message_Mutable...`: Get a mutable pointer to a repeated field or map, or reify a singular message field.

### 2.4. Object Cache Implementation

*   XS hash tables should be used.
*   **Keys:** The C pointer value (e.g., `(void*)upb_MessageDef`).
*   **Values:** The Perl `SV*` wrapper. Weak references might be necessary if Perl objects don't otherwise hold the only reference.
*   The `Protobuf::DescriptorPool` can host the cache for `upb_Def*` pointers.
*   Caches for `upb_Message*` might be associated with the arena or the top-level message. The Python UPB extension uses a per-interpreter global cache.

### 2.5. Type Conversion (`xs/types.c`)

*   **SV to UPB:** Ensure strings/bytes are copied onto the *destination message's arena*.
*   **UPB to SV:** For message types, use the object cache to get/create the wrapper SV.

By adopting object caching and the stub pattern, the Perl implementation can achieve similar efficiency and memory benefits as the Python UPB extension.