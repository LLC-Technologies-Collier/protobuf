# Core Architecture

[TOC]

<!-- Section: 030-Memory-Management.md -->

## Architecture: Memory Management

### C Layer (upb)

1.  **Arenas:** The `upb` library's arena allocation model will be used
    extensively to manage the lifetime of messages, descriptors, and related C
    structures.
2.  **Ownership:** C structures created and returned to Perl will typically be
    owned by the arena associated with the parent Perl object (e.g., a Message
    object's arena).

### Advanced Memory Management

1.  **Thread-Local Arena Caching (VPP Pattern)**: (Implemented) To optimize
    ultra-high-frequency allocations of small messages, the implementation
    supports thread-local arena caching. Inspired by the **Vector Packet
    Processor (VPP)** architecture, this pattern keeps a "warm" arena per
    interpreter (stored in the Registry) to ensure that message memory resides
    in the L1/L2 data cache.
2.  **Vectorized Processing**: (Planned) Implement a "Batch Parse" API that
    ingests a vector of incoming binary blobs. By processing messages in batches
    through a single graph node (parsing logic), we minimize instruction cache
    (I-cache) thrashing and maximize hardware pre-fetcher efficiency.
3.  **Zero-Copy IPC**: (Implemented) Support for `tmpfs`-backed custom
    allocators allows `upb_Arena` to reside in shared memory segments for
    zero-copy high-performance inter-process communication, mimicking VPP's
    zero-copy packet handoff between graph nodes.
4.  **Engineering Excellence (Reach for More):**
    -   **NUMA-Aware Allocation:** Optimize memory placement for multi-socket
        systems by ensuring arena blocks are allocated on the same NUMA node as
        the executing thread.
    -   **Audit-log Driven Leak Sanitizer:** Utilize the internal circular audit
        log to track sub-arena allocations and identify specific leak patterns
        without the overhead of full valgrind/asan instrumentation.
    -   **O(1) Fingerprinted Dispatch:** Implement stable hash-based message
        type identification for faster dispatch in high-frequency streams.
5.  **Safety Guards**: (Implemented) Memory corruption guards (canary bytes) are
    placed around arena-allocated blocks. A 16-byte pattern
    (`0xDEADBEEFCAFEBABEULL`) is written to the start and end of every block.
    These canaries are verified during `free` and `realloc` operations, and upon
    arena destruction, to detect out-of-bounds writes.
6.  **Observability**: (Implemented) Observation APIs report detailed arena
    memory usage statistics (Allocated, Reserved, and Block count). See
    `PerlUpb_Arena_GetStats`.
7.  **Merging Efficiency**: (Implemented) High-performance message merging
    (`parse_from`) is architected to utilize arena sharing, allowing data from
    one message tree to be integrated into another with minimal new allocations.
    Verified basic arena-sharing integrity in C integration tests.
8.  **Arena Fusion**: (Planned) Implement a high-performance "fusion" operation
    at the Perl level, allowing two independent arenas to be merged into a
    single logical lifecycle to support efficient message tree migration.
9.  **Custom Allocators**: (Planned) Extend `Protobuf::Arena` to support
    specialized allocator backends, including fixed-size pre-allocated pools for
    real-time systems.

--------------------------------------------------------------------------------

<!-- Section: core/02-memory-management.md -->

## Memory Management

The Perl Protobuf implementation, like its Ruby and PHP counterparts, will
heavily rely on `upb`'s arena-based memory management.

### Key Concepts

1.  **`upb_Arena` Ownership for Messages:** Each top-level Perl
    `Protobuf::Message` object instance will own a `upb_Arena`. This arena is
    created when the message object is instantiated (via lazy builder) and freed
    in `DEMOLISH`.
2.  **`upb_Arena` Ownership for Pools:** Each `Protobuf::DescriptorPool`
    instance also owns a `upb_Arena`. This arena is used for parsing
    `FileDescriptorSet` messages and for the storage of the C `upb_DefPool` and
    all definitions it loads. It's created via a lazy builder and freed in
    `DEMOLISH`.
3.  **Lifecycle Management:**
    *   The `Protobuf::Message` arena's lifecycle is tied to the Perl message
        object.
    *   The `Protobuf::DescriptorPool` arena's lifecycle is tied to the pool
        object.
    *   **Crucial:** `Protobuf::Descriptor` objects, which contain raw pointers
        to C definitions (like `upb_MessageDef*`), *must* hold a strong
        reference to the `Protobuf::DescriptorPool` instance that owns the
        underlying C `upb_DefPool`. This prevents the pool from being destroyed
        prematurely.
4.  **Submessages & Arenas:** When a message field is accessed, the returned
    `Protobuf::Message` object (whether a new stub or a reified message) uses
    the *parent's* arena. All upb C functions for field access
    (`upb_Message_Set...`, `upb_Message_Get...`, `upb_Message_Mutable`) handle
    the arena management internally. There is no need for explicit arena fusing
    in the XS layer.
5.  **Generalized Block Allocators:** To support specialized performance needs
    (like shared memory or thread-local caching), the implementation provides a
    generalized `PerlUpb_BlockAlloc` structure. This is a custom `upb_alloc`
    implementation that manages a contiguous memory block.
    *   **MMAP Blocks:** Used for `tmpfs` zero-copy IPC. The memory is backed by
        a shared file.
    *   **MALLOC Blocks:** Used for RAM-backed linear arenas (e.g., for
        thread-local "fast-path" caching).
    *   **Abstraction:** The `PerlUpb_Arena_NewBlock` helper handles the
        creation of these specialized arenas, hiding the allocation details from
        the rest of the C layer.
    *   **Memory Canaries:** Both `MMAP` and `MALLOC` blocks are protected by
        16-byte canary patterns (`0xDEADBEEFCAFEBABEULL`) written at the
        boundaries of every allocation. Corruption is detected during
        reallocations or when the allocator's `free` function is called.
6.  **Object Caching:** To ensure object identity and efficiency, the XS layer
    should maintain caches (e.g., hash tables) mapping UPB C pointers to their
    corresponding Perl wrapper SVs. This applies to:
    *   `upb_MessageDef*`, `upb_EnumDef*`, etc. -> `Protobuf::Descriptor` SVs
        (cached within the pool).
    *   `upb_Message*` -> `Protobuf::Message` SVs (cached on the arena or parent
        message). Functions returning wrappers must consult and update these
        caches.
7.  **Message Stubs for Singular Fields:** An unset singular message field will
    not initially have a `upb_Message` allocated. Accessing such a field will
    return a lightweight "stub" `Protobuf::Message` Perl object. This stub holds
    a reference to the parent message and the `upb_FieldDef*`. Only when a field
    within the stub is *modified* will the `upb_Message` be allocated on the
    parent's arena, and the stub becomes fully "reified".

### Perl Implementation

-   `Protobuf::Message` and `Protobuf::DescriptorPool` manage their respective
    `upb_Arena*` as integer attributes in the Perl hash, handled by XS builder
    and `DEMOLISH` methods.
-   `Protobuf::Descriptor` holds a `_pool` attribute which is a reference to the
    creating `Protobuf::DescriptorPool`.
-   XS functions for creating/accessing descriptors and messages will use hash
    tables to cache and retrieve Perl wrappers for C pointers.
-   The `xs/types.c` conversion layer will ensure any data copied from Perl
    (like strings) is allocated on the correct `upb_Arena` provided to the
    conversion functions.

--------------------------------------------------------------------------------

<!-- Section: advanced/01-tmpfs-ipc-design.md -->

## High-Performance Shared Memory IPC Design (tmpfs + SELinux)

### Overview

This document describes the architecture for an ultra-high-performance
Inter-Process Communication (IPC) mechanism for Protocol Buffers in Perl. It
leverages the Linux kernel's `tmpfs` (memory-backed filesystem) for
zero-network-overhead data transfer and SELinux for fine-grained Mandatory
Access Control (MAC).

### Architectural Components

#### 1. The Transport Layer: `tmpfs`

Instead of serializing messages into a TCP or Unix domain socket, messages are
serialized directly into a memory-mapped file residing in a `tmpfs` mount
(typically `/dev/shm`).

*   **VPP-Style Graph Handoff:** This mechanism mirrors the **Vector Packet
    Processor (VPP)** architecture's zero-copy handoff. By moving the pointer to
    the shared memory block instead of the data itself, we eliminate kernel
    protocol stack overhead and context switches.
*   **Mapping:** Processes `mmap()` a shared file.
*   **Speed:** Data transfer happens at memory bandwidth speeds.
*   **State:** The file contains the **serialized wire format** of the Protobuf
    message, not the raw C structures (to avoid address space layout
    randomization issues with C pointers).

#### 2. The Signaling Mechanism: `eventfd` or Unix Sockets

Since `tmpfs` does not notify readers of writes, a lightweight signaling
mechanism is required.

*   **Primary choice:** `eventfd` for single-bit synchronization.
*   **Fallback:** A Unix domain socket sending a small notification (e.g., the
    offset and length of the new message).

#### 3. The Security Model: SELinux & POSIX

The "Auth-Aware" nature of this pool is enforced by the kernel.

*   **POSIX Permissions:** Standard `u+r, g+r` bits ensure that only the correct
    UID/GID can access the file.
*   **SELinux Contexts:** Every memory block (file in `/dev/shm`) is assigned an
    SELinux label (e.g., `protobuf_auth_t`).
    *   The `auth-daemon` process is granted `read/write` access to
        `protobuf_auth_t`.
    *   The `web-worker` process is granted `read-only` access to
        `protobuf_auth_t`.
    *   The kernel blocks any other process from even seeing the existence of
        the memory block.

### Implementation Plan (Harness)

1.  **Harness Process:** A Perl script which:

    *   Creates a 1MB file in `/dev/shm/proto_ipc`.
    *   Spawns a **Perl Worker** and a **Python Worker**.
    *   Passes the file descriptor and signaling socket to both.

2.  **Workflow:**

    *   **Perl Worker** populates a complex `TestAllTypes` message.
    *   **Perl Worker** serializes it into the shared memory block.
    *   **Perl Worker** sends a "Ready" signal.
    *   **Python Worker** receives the signal, reads from the shared memory, and
        parses it.
    *   **Python Worker** validates the contents and sends an "ACK" signal back.

### Fuzzing Strategy

The harness will include a "Chaos Mode" where it writes garbage into the shared
memory block to ensure that both the Perl and Python UPB runtimes fail
gracefully (croak) instead of segfaulting.

*   **Malformed headers:** Change wire types to invalid values.
*   **Truncated data:** Send signal before write is complete.
*   **Memory corruption:** Randomly flip bits in the shared block.

### Advanced Optimization Goals

To achieve world-class IPC performance and safety, the implementation includes
(or is planned to include) the following:

-   **Zero-Copy Deserialization**: Implement
    `Protobuf::Arena->attach_message(name, offset)` which reifies a message
    directly from the shared memory block without copying data into intermediate
    Perl SVs.
-   **Security Verification**: Use `Protobuf::Arena->verify_selinux()` to ensure
    the shared memory segment complies with security policies before access.
-   **Handle Serialization**: Use `Protobuf::Arena->get_path()` and
    `Protobuf::Arena->get_offset(ptr)` to exchange message locations between
    processes.
-   **Cross-Process Identity**: Use `$message->get_fingerprint()` to obtain a
    unique `path:offset` identifier, allowing disparate processes to confirm
    they are looking at the same memory instance without expensive deep-equality
    checks.
-   **Integrated Fuzzer Verification**: (Planned) Every IPC release MUST be
    verified against a continuous fuzzing harness that simulates malicious
    shared memory state.
-   **Multi-Language Handshake**: (Planned) Define a standard "shared memory
    handshake" protocol to allow the Perl implementation to discover and
    negotiate capabilities with official Python and C++ runtimes sharing the
    same pool.

### Future Evolution (Reach for More)

-   **NUMA-Aware Allocation Balance**: (Planned) Distribute arena blocks across
    NUMA nodes based on load to maximize memory bandwidth in high-core systems.
-   **COW-Optimized Shared Cache**: (Planned) Utilize Copy-On-Write (COW) memory
    mapping for the shared object cache, enabling massive read scaling with
    near-zero memory footprint for replicated workers.
-   **SIMD-Accelerated Integrity Scanning**: (Planned) Utilize AVX-512
    instructions to scan large shared arenas for canary corruption in parallel,
    minimizing the latency of frequent integrity audits.

### Conclusion

This design provides a blueprint for a production-ready, secure, and extremely
fast IPC mechanism that bypasses the limitations of the Perl interpreter's
threading model.

--------------------------------------------------------------------------------

<!-- Section: core/01-upb-interfacing.md -->

## Interfacing with UPB from Perl XS

This document details how the Perl XS layer should interact with the `upb` C
library, drawing on patterns observed in the PHP and Python `upb` extensions.

### 1. Core Principles

*   **Arenas are Fundamental:** All allocations of `upb` objects (messages,
    arrays, maps) and their data (strings, bytes) MUST happen within a
    `upb_Arena`.
*   **Ownership:** The Perl wrapper objects (`Protobuf::Message`,
    `Protobuf::DescriptorPool`, etc.) OWN their primary `upb_Arena` instances.
    Destruction of the Perl object (in `DEMOLISH`) triggers the freeing of the
    associated arena.
*   **Lifecycle Management:** References between Perl objects are crucial.
    `Protobuf::Descriptor` MUST hold a reference to its
    `Protobuf::DescriptorPool`. Stub messages MUST hold a reference to their
    parent message.
*   **MiniTables:** `upb` operations on messages heavily rely on `upb_MiniTable`
    structures.
*   **Error Handling:** Check return values and `upb_Status` from `upb`
    functions and propagate errors to Perl using `croak`.
*   **Object Caching:** To ensure that the same C pointer (e.g.,
    `upb_MessageDef*`, `upb_Message*`) always yields the same Perl object (SV),
    a global per-interpreter cache is used. This is essential for object
    identity and performance.
*   **Undef Behavior:** Unlike some implementations that use "Stub" objects,
    this implementation returns **`undef`** for unset singular message fields.
    Accessing a sub-message getter does not automatically instantiate it unless
    it already exists on the wire or has been explicitly set.

### 2. Mirroring the Python Protocol Buffers Implementation

To maximize code reuse and benefit from the design choices made in the mature
Python UPB implementation, the Perl Protocol Buffers implementation should
closely mirror its structure, both at the C-level and the Perl-level.

*   **C File Organization (`perl/xs/`):** The C files in `perl/xs/` are
    organized into subdirectories (e.g., `perl/xs/message/`,
    `perl/xs/descriptor/`) corresponding to the major components, mirroring the
    layout of the `python/` directory in the main protobuf project. Within these
    subdirectories, files are further broken down (e.g.,
    `perl/xs/descriptor/message.c`, `perl/xs/descriptor/enum.c`). This layer
    directly interfaces with the `libupb` libraries. See
    [XS & C File Organization](OVERVIEW.md#xs--c-file-organization).

*   **Perl Module Organization (`perl/lib/Protobuf/`):** The Perl module files
    should be structured to mirror the Python modules in
    `python/google/protobuf/`. Core classes like `Message`, `Descriptor`,
    `DescriptorPool` in Python will have corresponding `Protobuf::Message`,
    `Protobuf::Descriptor`, `Protobuf::DescriptorPool` modules in Perl.

*   **Naming Conventions:**

    *   **Perl Modules:** Use the `Protobuf::` namespace for user-facing
        classes.
    *   **C Functions/Types:** Internal C types and functions within the XS
        layer should be named analogously to their Python C API counterparts
        (e.g., functions in `perl/xs/descriptor.c` will be similar to those in
        `python/descriptor.c`).

*   **Adaptation:** While the core logic and structure will be mirrored, a
    straight port is not possible.

    *   **C Layer:** The C code in `perl/xs/` will use Perl's API (e.g., `SV*`,
        `HV*`, `newSViv`, `hv_store`) instead of Python's C API (PyObject*,
        PyDict_, etc.).
    *   **Perl Layer:** The `.pm` files will use Perl idioms (Moo, XS, etc.) to
        provide the high-level API, while the underlying implementation in C
        will be structurally similar to the Python version's C code.

This approach aims to reduce the risk of design flaws, enhance maintainability
by providing a consistent structure across language implementations, and benefit
from the existing work done for the Python bindings.

### 4. Coro Safety Considerations

When writing XS code that interacts with the Perl interpreter, special care must
be taken to ensure Coro safety. Since Coro works by saving and switching Perl
interpreter contexts, any C code that can be called from Perl must be re-entrant
and avoid unsafe access to shared C global data.

*   **Avoid C Globals:** Do not use static or global variables in C to store
    state that is not constant. If state must be shared, use Perl's features or
    thread-local storage if appropriate (though standard C tls is not aware of
    Coro threads).
*   **Perl API:** Be mindful of functions that interact with the Perl stack or
    interpreter state. Generally, standard XS macros and functions are safe if
    used correctly.
*   **Re-entrancy:** Ensure that C functions can be called again while a
    previous invocation on the same data structures is in progress in a
    different Coro thread. This is particularly important for functions that
    modify shared state, like the object cache.
*   **UPB and Arenas:** UPB's arena-based allocation is generally Coro-safe, as
    arenas are typically confined to a single message tree or pool. Problems
    arise if C code outside of UPB introduces shared mutable state.

#### 2.1. Arena Management

*   **Creation:** `upb_Arena_New()`: Called in the XS builders for top-level
    `Protobuf::Message` and `Protobuf::DescriptorPool`.
*   **Freeing:** `upb_Arena_Free()`: Called in `DEMOLISH`.
*   **Fusing:** Arena fusing is handled *internally* by UPB's message accessor
    functions. When you set a submessage, UPB takes care of any necessary memory
    management on the parent's arena.
*   **Implementation:** The C functions for wrapping and managing `upb_Arena`
    (e.g., `PerlUpb_Arena_New`, `PerlUpb_Arena_Get`) are located in
    `xs/protobuf.c`.

#### 2.2. Descriptor Pool Operations (`upb_DefPool`)

*   **Loading Descriptors:** `upb_DefPool_AddFile()`: Used in
    `DescriptorPool.xs`.
*   **Finding Definitions:** `upb_DefPool_FindMessageByName()`, etc. The
    returned C pointers should be used as keys to the object cache to get/create
    the `Protobuf::Descriptor` Perl wrapper.

#### 2.6. Descriptor Accessors

To facilitate property access from the Perl layer, a set of C-level accessors is
provided. These functions wrap the underlying `upb` calls and return data in a
form ready for Perl (e.g., as `SV*`).

*   **MessageDef:**
    *   `PerlUpb_MessageDef_FullName(pTHX_ mdef)`: Returns the message's full
        name as a Perl SV.
    *   `PerlUpb_MessageDef_FindFieldByNameWithSize(pTHX_ mdef, name, len)`:
        Efficiently locates a field definition.
*   **FieldDef:**
    *   `PerlUpb_FieldDef_Name(pTHX_ fdef)`: Returns the field name as a Perl
        SV.
    *   `PerlUpb_FieldDef_Type(pTHX_ fdef)`: Returns the `upb_FieldType` as an
        integer.
    *   `PerlUpb_FieldDef_Label(pTHX_ fdef)`: Returns the `upb_Label` (e.g.,
        Optional, Required, Repeated) as an integer.

#### 2.3. Message Operations (`upb_Message`)

*   **Creation:**
    *   Top-level: `upb_Message_New()` on the message's own new arena.
    *   Submessage (when reifying a stub): `upb_Message_Mutable()` on the
        *parent's* arena.
*   **Field Access:** Use functions from `upb/message/accessors.h`. These
    functions handle arena operations for strings and submessages.
    *   `upb_Message_Get...`: Retrieve field values. For message fields, this
        might return a C pointer that needs to be wrapped in a Perl stub if not
        already cached.
    *   `upb_Message_Set...`: Set field values.
    *   `upb_Message_Has...`: Check field presence.
    *   `upb_Message_Clear...`: Clear fields.
    *   `upb_Message_Mutable...`: Get a mutable pointer to a repeated field or
        map, or reify a singular message field.

#### 2.4. Object Cache Implementation

*   XS hash tables should be used.
*   **Keys:** The C pointer value (e.g., `(void*)upb_MessageDef`).
*   **Values:** The Perl `SV*` wrapper. Weak references might be necessary if
    Perl objects don't otherwise hold the only reference.
*   The `Protobuf::DescriptorPool` can host the cache for `upb_Def*` pointers.
*   Caches for `upb_Message*` might be associated with the arena or the
    top-level message. The Python UPB extension uses a per-interpreter global
    cache.

#### 2.5. Type Conversion (`xs/types.c`)

*   **SV to UPB:** Ensure strings/bytes are copied onto the *destination
    message's arena*. (Planned) Implement range-validated strict type checking
    for narrowing conversions.
*   **UPB to SV:** For message types, use the global object cache to get/create
    the wrapper SV. Ensure sub-messages share their parent's arena by passing
    the parent's `arena_sv` to `PerlUpb_WrapMessage`. (Planned) Support
    transparent Math::BigInt promotion for 64-bit integer overflows.

### 3. Advanced Integration and Conversion

To achieve world-class reliability, the integration of conversion and core
utilities follows these principles:

-   **Exhaustive Roundtripping**: Every primitive type, repeated field, and map
    is verified for bit-exact roundtripping through integrated C-level tests.
-   **Integrated Cache Identity**: The conversion layer (`UpbToSv`) is strictly
    integrated with the `ObjCache`. Converting the same sub-message pointer
    multiple times (e.g., via different parent paths) MUST return the same Perl
    SV.
-   **Lock-Free Multi-Access**: Read-only operations are architected to be
    lock-free, enabling high-performance concurrent access from multiple Perl
    coroutines (e.g. in Mojo or Coro).

--------------------------------------------------------------------------------

<!-- Section: core/03-object-caching.md -->

## Object Caching

*Status: Fully Implemented*

To ensure object identity and improve performance, the implementation uses a
global (per-interpreter) cache to map underlying UPB C objects (pointers) to
their corresponding Perl wrapper SVs.

### Cache Mechanism

The C implementation of the object cache is located in
`perl/xs/protobuf/obj_cache.c` and `perl/xs/protobuf/obj_cache.h`.

*   **Registry-Based Storage:** To eliminate expensive global Perl SV lookups
    (`get_sv`), the Object Cache (`HV*`), LRU array (`AV*`), and Audit Log are
    stored within a centralized **Per-Interpreter Registry**
    (`PerlUpb_Registry`).
*   **Keys:** Hexadecimal string representations of C pointer addresses (e.g.,
    `0x7fd1a2b3c4d5`).
*   **Values:** Weak references (using `sv_rvweaken`) to the blessed Perl
    objects.

### Concurrency and Scalability

The cache is designed for high-throughput, multi-threaded environments
(ithreads/Coro):

1.  **Striped Locking:** Replaces a single global lock with a **16-stripe
    mutex** array. Pointers are hashed to a specific stripe to minimize
    contention during concurrent access.
2.  **Lock Abstraction:** Uses `perl/xs/protobuf/port.h` to provide portable
    mutex macros (`PERL_PROTOBUF_MUTEX_LOCK`, etc.) that utilize Perl's native
    mutexes when `USE_ITHREADS` is defined.
3.  **Interpreter Isolation:** The Registry is stored in the interpreter's
    `PL_modglobal` hash. This ensures that every thread/coroutine has
    zero-contention access to its own state.

### LRU Eviction

To prevent unbounded memory growth, the cache implements a FIFO-based eviction
strategy:

*   **Capacity:** Configurable via `Protobuf::Internal::set_cache_capacity()`
    (Default: 100,000).
*   **LRU Tracking:** A Perl array (`AV*`) in `Protobuf::_obj_lru` tracks
    insertion order.
*   **Eviction Loop:** When capacity is exceeded, the oldest keys are shifted
    from the array and deleted from the hash. The logic is stripe-aware and
    verifies existence before eviction to handle stale entries safely.

### High-Performance Audit Log

The cache includes a per-interpreter circular ring buffer for real-time
observability:

*   **Event Types:** `ADD`, `HIT`, `MISS`, `DELETE`, `EVICT`.
*   **Performance:** O(1) logging with fixed memory overhead (~32KB per
    interpreter).
*   **Access:** Exposed via `Protobuf::Internal::get_cache_audit_log()`.

### Usage in XS

Utilize the `RETURN_CACHED_OR_CREATE_BLESSED` macro in `xs/descriptor/base.h` or
`PerlUpb_WrapArenaBoundObject` in `xs/protobuf/utils.c` to automatically handle
cache lookups and insertions.

--------------------------------------------------------------------------------

<!-- Section: core/object-caching.md -->

## Object Caching

To ensure object identity and performance, we will implement a caching mechanism
similar to the Python UPB extension. This cache will map underlying C pointers
(e.g., `upb_MessageDef*`, `upb_Message*`, `upb_Arena*`) to their corresponding
Perl wrapper objects.

### Cache Implementation

The cache will likely be implemented as a Perl hash. The keys will be the memory
addresses of the C pointers (cast to integers), and the values will be weak
references to the Perl objects.

-   **Weak References:** Using `WeakRef` is crucial to prevent the cache from
    keeping objects alive longer than necessary. When the last non-weak
    reference to a Perl object is gone, the object will be destroyed, and the
    weak reference in the cache will become undefined.

-   **Scope:** The cache could be global, or more appropriately, tied to a
    `Protobuf::Arena` instance. Since `upb` objects are arena-allocated, a
    per-arena cache makes sense. Objects from different arenas should not be
    mixed.

### Cache Usage

When a C function returns a `upb` pointer that needs to be exposed to Perl:

1.  Check if the pointer exists as a key in the cache.
2.  If found, return the cached Perl object.
3.  If not found, create a new Perl wrapper object, store it in the cache with
    the C pointer as the key, and then return the new object.

This applies to:

-   Descriptors (`upb_MessageDef*`, `upb_FieldDef*`, etc.)
-   Messages (`upb_Message*`)
-   Arenas (`upb_Arena*`)

This strategy ensures that multiple requests for the same underlying C object
always return the same Perl object, maintaining referential integrity.

--------------------------------------------------------------------------------

<!-- Section: core/04-perl-object-structure.md -->

## Perl Object Structure

Perl objects wrapping Protobuf messages are designed to be lightweight and
delegate most operations to the XS layer, which interfaces with the `upb`
library.

### Message Object (`Protobuf::Message`)

A Perl object of a specific message type (e.g., `test::TestMessage`) is a
blessed hash.

*   **`upb_msg`**: An opaque pointer (stored as an IV) to the `upb_Message`
    struct in C.
*   **`arena_sv`**: A reference to the `Protobuf::Arena` (or the parent's
    `arena_sv`) that owns the memory for this message. This keeps the arena
    alive as long as any message or sub-message referencing it exists.
*   **`_descriptor`**: An opaque pointer to the `upb_MessageDef` (C definition).

#### Sub-Message Behavior

Unlike some implementations that use "Stub" objects, this implementation returns
**`undef`** for unset singular message fields. Accessing a sub-message getter
does not automatically instantiate it unless it already exists on the wire or
has been explicitly set.

### Descriptor Objects (`Protobuf::Descriptor::*`)

Descriptor objects (e.g., `Protobuf::Descriptor::MessageDef`,
`Protobuf::Descriptor::Field`) follow a similar structure:

1.  **`_ptr`**: Opaque pointer to the C definition (e.g., `const
    upb_MessageDef*`).
2.  **`_pool`**: A reference to the `Protobuf::DescriptorPool` instance that
    owns the definition, ensuring the C memory remains valid.

### Field Access

Instead of using `AUTOLOAD`, which can be slow and less transparent, this
project uses **dynamic method injection** via `Protobuf::ClassGenerator`.

-   **Method Injection**: When a descriptor is loaded,
    `Protobuf::ClassGenerator` injects getters (`fieldname`), setters
    (`set_fieldname`), presence checkers (`has_fieldname`), and clearers
    (`clear_fieldname`) directly into the generated message class.
-   **Dispatch to XS**: These generated methods call lightweight wrapper methods
    (`get`, `set`, etc.) in `Protobuf::Message`, which then dispatch to the XS
    layer.
-   **XS Operations**: The XS layer uses the stored `upb_msg` and the field's
    `upb_FieldDef` to interact with the underlying `upb` data.

--------------------------------------------------------------------------------

<!-- Section: core/05-type-conversion.md -->

## Type Conversion

Marshalling data between Perl's Scalar Values (SVs) and `upb`'s C types (a
`upb_MessageValue` union) is a critical function of the XS layer.

### Key Conversions

XS functions will be needed to handle conversions for each Protocol Buffer type:

| Proto Type | C Type (in          | Perl Type (SV)            | Conversion    |
:            : `upb_MessageValue`) :                           : Notes         :
| ---------- | ------------------- | ------------------------- | ------------- |
| int32,     | `int32_t`           | IV                        | Check for     |
: sint32     :                     :                           : overflow.     :
| uint32,    | `uint32_t`          | UV                        | Check for     |
: fixed32    :                     :                           : overflow.     :
| int64,     | `int64_t`           | IV (if fits), string      | Use functions |
: sint64     :                     :                           : like `SvIV`,  :
:            :                     :                           : `SvUIV`,      :
:            :                     :                           : potentially   :
:            :                     :                           : lossy for     :
:            :                     :                           : very large    :
:            :                     :                           : numbers.      :
| uint64,    | `uint64_t`          | UV (if fits), string      | Use functions |
: fixed64    :                     :                           : like `SvUV`,  :
:            :                     :                           : potentially   :
:            :                     :                           : lossy.        :
| float      | `float`             | NV                        | Standard      |
:            :                     :                           : float/double  :
:            :                     :                           : conversion.   :
| double     | `double`            | NV                        | Standard      |
:            :                     :                           : float/double  :
:            :                     :                           : conversion.   :
| bool       | `bool`              | bool                      | `SvTRUE` /    |
:            :                     :                           : `SvFALSE`.    :
| string     | `upb_StringView`    | PV (string)               | Ensure UTF-8  |
:            :                     :                           : encoding.     :
:            :                     :                           : Data copied   :
:            :                     :                           : into the      :
:            :                     :                           : arena.        :
| bytes      | `upb_StringView`    | PV (string)               | No encoding   |
:            :                     :                           : checks. Data  :
:            :                     :                           : copied into   :
:            :                     :                           : the arena.    :
| enum       | `int32_t`           | IV                        | Integer value |
:            :                     :                           : of the enum.  :
| message    | `const              | Blessed Ref               | Pointer to    |
:            : upb_Message*`       :                           : the           :
:            :                     :                           : `upb_Message` :
:            :                     :                           : instance.     :
:            :                     :                           : Arena fusing  :
:            :                     :                           : is critical.  :
| repeated   | `const upb_Array*`  | `Protobuf::RepeatedField` | Perl wrapper  |
:            :                     :                           : object.       :
| map        | `const upb_Map*`    | `Protobuf::Map`           | Perl wrapper  |
:            :                     :                           : object.       :

### XS Implementation

-   A series of functions like `sv_to_upb_T()` and `upb_to_sv_T()` will be
    created for each type `T`.
-   These functions will handle type checking on the Perl side (e.g., using
    `SvIOK`, `SvNOK`, `SvPOK`).
-   String/bytes data from Perl SVs will be copied into the `upb_Arena` to
    ensure its lifetime is tied to the message.
-   When converting from `upb` to Perl, new SVs will be created. For strings and
    bytes, the SV will likely copy the data from the arena.

### Encoding

-   Perl strings used for Protobuf `string` fields MUST be valid UTF-8. XS code
    uses `SvPVutf8` to ensure correct decoding.
-   Data for `bytes` fields will be treated as raw octets, accessed via
    `SvPVbyte`.
-   This follows best practices for handling Perl strings in XS to avoid
    encoding issues, as highlighted by community feedback.

### Advanced Conversion Goals

To achieve world-class performance and robustness, the conversion layer includes
(or is planned to include) the following:

-   **Math::BigInt Support**: (Implemented) 64-bit integers (`int64`, `uint64`,
    `fixed64`, `sfixed64`, `sint64`) that exceed the native Perl IV/UV range are
    transparently promoted to `Math::BigInt` objects. This ensures no precision
    loss for large IDs or timestamps.
-   **SIMD Acceleration**: (Implemented) Leveraging hardware acceleration
    (SSE4.2/AVX2) via `third_party/utf8_range` for fast UTF-8 validation of
    incoming Perl strings, significantly reducing the overhead of
    high-throughput string ingestion.
-   **Engineering Excellence (Reach for More):**
    -   **Zero-Copy ByteBuffer Projections:** Implement `mmap`-backed scalars
        for large `bytes`/`string` fields, allowing Perl to manipulate the
        underlying `upb` message buffer without copying memory.
    -   **O(1) Map Identity Projection:** Direct hash-table layout mapping
        between `upb_Map` and native Perl hashes for near-instant conversion of
        large map fields.
    -   **Strict Range Validation:** Implementation of range-checking for
        narrowing conversions (e.g., Perl number to `int32`), providing
        predictable error behavior for type-safety.
    -   **Type/Boundary Fuzzing:** Automated verification of conversion logic
        against incompatible SV types and integer edge cases (MIN/MAX) to ensure
        absolute memory safety and consistent reporting.

--------------------------------------------------------------------------------

<!-- Section: 010-Error-Handling.md -->

## Architecture: Error Handling

### C Layer (XS)

1.  **upb Errors:** Errors originating from the `upb` library functions should
    be checked meticulously.
2.  **Propagation to Perl:** C functions called from Perl (XS) MUST have a clear
    way to signal errors back to the Perl layer. This will primarily be done by
    returning `NULL` or a special sentinel value for functions expected to
    return pointers, and a dedicated boolean/integer status for others.
3.  **Perl Exceptions:** Upon detecting an error from `upb` or internal C logic,
    the XS wrapper code should construct a descriptive error message and use
    `croak()` or the `PerlUpb_Error_Die()` utility to throw a Perl exception.
4.  **Error Messages:** Error messages should be informative, including the
    function that failed and the reason, if available from `upb`.
5.  **Utilities:** The `PerlUpb_Error_Die()` function provides a standardized
    way to throw Perl exceptions from within C utility functions, ensuring
    consistent error reporting across the C layer.

### Perl Layer

1.  **Exception Based:** The Perl API will use exceptions (die/croak) to signal
    error conditions.
2.  **Specific Error Classes:** (Consideration for the future) We may introduce
    specific error classes inheriting from a base `Protobuf::Error` to allow for
    more granular error handling by users.

--------------------------------------------------------------------------------

<!-- Section: core/06-error-propagation.md -->

## Error Propagation

Errors originating from the `upb` library or within the XS glue code must be
propagated to the Perl level as exceptions.

### Mechanisms

1.  **`upb_Status`:** Many `upb` functions, especially those involved in parsing
    or complex operations, accept a `upb_Status*` argument. After calling such
    functions, the XS code MUST check the status using `upb_Status_IsOk()`.
    -   If not OK, an error message can be retrieved using
        `upb_Status_ErrorMessage()`. This message should be used to construct an
        error string for Perl.
2.  **Return Values:** Other `upb` functions indicate errors through return
    values (e.g., returning `NULL`). These must be checked.
3.  **`croak`:** The standard mechanism for signaling errors from XS to Perl is
    `croak()`. XS functions should use `croak()` to throw Perl exceptions. The
    message passed to `croak()` should be informative, including the error
    message from `upb_Status` if available.

### Example XS Error Handling:

```c
// Inside an XSUB
upb_Status status;
upb_Status_Clear(&status);
bool ok = upb_SomeOperation(..., &status);
if (!ok) {
    const char* err_msg = upb_Status_ErrorMessage(&status);
    croak("Protobuf error: %s", err_msg);
}

void* ptr = upb_AnotherOperation(...);
if (!ptr) {
    croak("Protobuf error: AnotherOperation failed");
}
```

### Perl-level Catching

Perl code using the `Protobuf` module can use `eval { ... }` blocks or modules
like `Try::Tiny` to catch these exceptions:

```perl
use Try::Tiny;

try {
    my $msg = MyMessage->decode($binary_data);
} catch {
    warn "Failed to decode message: $_";
};
```

This error handling strategy is consistent with both the Ruby (using `rb_raise`)
and PHP (using `zend_throw_exception_ex`) extensions, providing clear and
idiomatic error reporting to the Perl user.

--------------------------------------------------------------------------------

<!-- Section: core/07-xs-file-organization.md -->

## XS File Organization

To enhance maintainability and align with the structure of the Python UPB
extension, the C code for the Perl XS layer is organized into subdirectories
within `perl/xs/`.

### Directory Structure

Each major component of the Protobuf implementation has its own subdirectory:

-   `perl/xs/convert/`: Type conversion routines.
-   `perl/xs/descriptor/`: Descriptor wrappers and logic.
-   `perl/xs/descriptor_containers/`: Generic containers for descriptor
    collections.
-   `perl/xs/descriptor_pool/`: DescriptorPool implementation.
-   `perl/xs/extension_dict/`: ExtensionDict implementation.
-   `perl/xs/map/`: Map field implementation.
-   `perl/xs/message/`: Message object implementation.
-   `perl/xs/protobuf/`: Core module utilities, Arena wrapper, Object Cache.
-   `perl/xs/repeated/`: Repeated field implementation.
-   `perl/xs/unknown_fields/`: UnknownFieldSet implementation.

Top-level files like `perl/xs/protobuf.h` may still exist for widely shared
declarations.

### Rationale

-   **Modularity:** Keeps code for different components separate and easier to
    manage.
-   **Alignment:** Makes it easier to reference and compare with the Python UPB
    C source code.
-   **Scalability:** Allows for breaking down large C files into smaller, more
    focused units within each component directory. For example,
    `perl/xs/descriptor/` contains `message.c`, `enum.c`, etc.

--------------------------------------------------------------------------------

<!-- Section: 070-XS-Internals.md -->

## Architecture: XS Internals & C API

### Purpose of `libprotobufperl.so`

`libprotobufperl.so` is a shared library containing the `upb` core, third-party
dependencies (`utf8_range`), and custom C helper functions specifically for this
Perl-UPB binding. This library centralizes common logic used across all XS
sub-modules (`Arena.so`, `Message.so`, etc.).

### Symbol Visibility and ABI Stability

To ensure ABI stability and prevent symbol collisions, we use a linker version
script (`libprotobufperl.map`) to strictly control which symbols are exported
from `libprotobufperl.so`.

*   **Global Symbols:** Only symbols within the `PerlUpb_*`, `upb_*`, `_upb_*`,
    `google_*`, and `utf8_range_*` namespaces are exported.
*   **Local Symbols:** All other symbols (including internal helpers and leaked
    `main` functions from test objects) are hidden from the global namespace
    using `local: *;`.

### C API Naming Conventions for XS

*   Functions intended for global export MUST be prefixed with `PerlUpb_`.
*   Functions intended to be called from XS should follow the
    `PerlUpb_<Component>_<Method>` pattern.
*   The top-level initialization function is
    `PerlUpb_Protobuf_InitModule(pTHX)`.

### Initialization Flow

1.  **XS Loading:** When the main `Protobuf.so` or any sub-module is loaded, it
    links against `libprotobufperl.so`.
2.  **BOOT Section:** The `BOOT:` section in `lib/Protobuf.xs` MUST call
    `PerlUpb_Protobuf_InitModule(aTHX)`.
3.  **Component Registration:** `PerlUpb_Protobuf_InitModule` initializes the
    **Per-Interpreter Registry**, object cache, audit log, and registers
    internal XS functions in the `Protobuf::Internal` namespace.

### Per-Interpreter Registry

To avoid expensive global Perl SV lookups (`get_sv`) in performance-critical C
paths, the implementation utilizes a centralized registry struct
(`PerlUpb_Registry`) stored in the interpreter's `PL_modglobal` hash.

*   **Header:** `xs/protobuf/registry.h`
*   **State Managed:** Object Cache (`HV*`), LRU List (`AV*`), Audit Log
    (`void*`), and global configuration (e.g., `max_cache_capacity`).
*   **Access:** XS functions SHOULD use `PerlUpb_Registry_Get(aTHX)` to retrieve
    the current interpreter's state. This pattern de-risks future feature
    implementation (like thread-local arena caching) by providing a single,
    type-safe C hook for all global state.

### Lock Contention Profiling

To ensure scalability in multi-threaded environments, the striped object cache
mutexes include built-in contention profiling.

*   **Implementation:** `LOCK_AND_PROFILE` macro utilizes
    `pthread_mutex_trylock` to detect immediate availability.
*   **Metrics:** Tracks `acquisitions` and `contentions` per stripe, as well as
    for global `lru` and `audit` locks.
*   **Visibility:** Stats are exposed to Perl via
    `Protobuf::Internal::get_contention_stats()`.

### Predictive Allocator

To minimize expensive reallocations for recurring message patterns, the
`StatsAlloc` engine tracks peak memory usage.

*   **Mechanism:** Maintains a `historical_max_size` per `StatsAlloc` instance.
*   **Optimization:** When acquiring a new arena through the Registry, the
    `historical_max_size` is used as a hint for `upb_Arena_Init` (capped at
    1MB), pre-allocating a single block large enough for the expected workload.

### Automated Race Detection (TSAN)

The project includes integrated support for ThreadSanitizer (TSAN) to detect
data races in the XS core and concurrent C integration tests.

*   **Usage:** Run `make test_tsan` to rebuild the library with
    `-fsanitize=thread` and execute the full test suite.
*   **Scope:** Validates thread-safety of the per-interpreter registry, object
    cache stripes, and shared memory arena accesses.

### NUMA-Aware Allocation Balance (Planned)

To maximize memory bandwidth on multi-socket systems, the allocator will aim to
distribute arena blocks across NUMA nodes.

*   **Topology Detection:** Utilize `libnuma` to detect NUMA nodes and CPU
    affinity.
*   **Policy:** Implement policies to allocate memory on the NUMA node local to
    the calling thread, or round-robin if affinity is not clear.
*   **Tuning:** Expose tunables via `Protobuf::Internal` to control NUMA
    awareness.

### COW-Optimized Shared Object Cache (Planned)

For scenarios with many read-heavy worker processes, a Copy-On-Write shared
object cache can significantly reduce memory footprint.

*   **Mechanism:** Use file-backed `mmap` with `MAP_PRIVATE`. A process
    populates the cache file. Workers map this file; writes trigger page copies.
*   **Cache Structure:** Design the on-disk cache format for efficient mmap
    access.
*   **Invalidation:** Address how updates would be propagated (e.g., versioning,
    new file).

### Generalized Block Allocators

To support high-performance allocation patterns (like zero-copy IPC and
thread-local caching), we utilize a generalized block allocator
(`PerlUpb_BlockAlloc`).

*   **Header:** `xs/protobuf/arena.h` (Internal logic in
    `xs/protobuf/arena_tmpfs.c`)
*   **Abstraction:** Wraps a contiguous memory region and provides a `upb_alloc`
    compliant interface.
*   **Backends:** Supports `PERL_UPB_BLOCK_MMAP` (for file-backed shared memory)
    and `PERL_UPB_BLOCK_MALLOC` (for RAM-backed local memory).
*   **Usage:** Used by the Arena Factory to acquire arenas with specific
    performance characteristics (e.g., `PerlUpb_Arena_NewBlock`).

### Memory Canaries

To detect memory corruption (buffer overflows/underflows) in
performance-critical C paths, all custom allocators (`StatsAlloc` and
`BlockAlloc`) implement canary guards.

*   **Pattern:** `0xDEADBEEFCAFEBABEULL` (16 bytes at start and end).
*   **Verification:** Performed automatically during `free`, `realloc`, and
    arena destruction by `PerlUpb_VerifyCanaries`.
*   **Safety:** Errors trigger a Perl `croak` with a descriptive message (e.g.,
    "MEMORY CORRUPTION DETECTED (Overflow)").
*   **Recovery:** Upon detecting corruption, the allocator instance is marked as
    `poisoned`. Subsequent allocation requests through the poisoned allocator
    will return `NULL`, preventing further damage. This is tested in
    `t/c/integration/030_protobuf.c`.

### Memory Protection (Guard Pages) (Planned)

To provide stronger guarantees against memory corruption, custom block
allocators can use `mmap` and `mprotect` to place read-only guard pages before
and after allocated blocks. Any attempt to write outside the bounds will result
in an immediate `SIGSEGV`.

#### Guard Page Implementation Strategy

1.  **Allocator Modification:** Modify `PerlUpb_BlockAlloc_Func`. When a new
    block is allocated via `mmap`, allocate two additional pages, one before and
    one after the requested size.
2.  **Protection:** Use `mprotect` with `PROT_NONE` on these two extra pages,
    making them inaccessible.
3.  **Alignment:** Ensure the requested block within the three pages is properly
    aligned. The total `mmap` size will be `page_size + requested_size +
    page_size`. The returned pointer will be offset by `page_size`.
4.  **Tear Down:** When freeing, `mprotect` the guard pages back to `PROT_READ |
    PROT_WRITE` before calling `munmap` on the entire region.
5.  **Environment Flag:** Control this feature with an environment variable like
    `PROTOBUF_PERL_USE_GUARD_PAGES`.

### Fuzzing Harness Integration (Planned)

To proactively discover edge cases and security vulnerabilities, key components
will be integrated with fuzzing engines:

*   **Targets:** Wire format parser, TextFormat parser, JSON parser, IPC message
    handling.
*   **Engines:** libFuzzer (LLVM) and AFL++.
*   **CI:** Fuzzing will be part of the continuous integration pipeline.

### SIMD-Accelerated Integrity Scanning (Planned)

Aligning with the **Vector Packet Processor (VPP)** philosophy, hot paths
utilize SIMD instructions where appropriate to maximize throughput.

*   **CPUID Dispatcher:** `PerlUpb_InitCpuFeatures()` detects hardware
    capabilities at runtime. Optimized kernels are selected dynamically based on
    available instruction sets (SSE4.1, AVX2).
*   **Name Conversion:** `PerlUpb_ClassNameToFullName` utilizes AVX2/SSE4.1
    instructions to process class names in bulk, accelerating the common case
    where no special characters are present.
*   **Batch Validation:** `PerlUpb_FieldVector` provides a C-level API for
    collecting field descriptors and values into contiguous vectors, enabling
    vectorized validation kernels (e.g., `PerlUpb_ValidateIntRange_SSE41`).
*   **Hardware Requirement:** Implementation includes scalar fallbacks for all
    optimized paths to ensure portability across heterogeneous environments.

### Typemap Strategy

*   **Location:** Typemap entries will be placed in the `perl/typemap` file.
*   **Organization:** Group typemaps by the C type they handle.
*   **Naming:** Use standard typemap conventions.
*   **Usage:** Utilize typemaps to automate the conversion between Perl SVs and
    C types like `upb_Arena *`, `upb_Message *`, `upb_MiniTable *`, etc., and
    various descriptor types.

### Data Passing Between Perl and C

*   **Input to C:** Functions taking string data from Perl should expect to
    receive a `char *` and potentially a length, typically derived from `SvPV`.
    The C function is responsible for copying the data into an arena if it needs
    to persist beyond the function call.
*   **Output to Perl:** Functions returning data to Perl should generally return
    new SVs created via `newSVpv`, `newSVuv`, etc., with their ownership passed
    to Perl. Memory for these SVs is managed by Perl's aREFCNT system.

--------------------------------------------------------------------------------

<!-- Section: ../internal/xs_learnings.md -->

## Perl XS Development Learnings (Protobuf Edition)

This document captures key lessons learned during the development of the Perl XS
module for Protocol Buffers, interfacing with the `upb` library.

### 1. Memory Management & Object Lifecycles

*   **Dangling Pointers are Easy to Create:** When Perl objects wrap C pointers,
    the lifecycle of the C data MUST be tied to the Perl object. If the C data
    is freed while Perl objects still hold pointers to it, SEGV is inevitable.
*   **Inter-Object References:** If a Perl object (e.g., `Protobuf::Descriptor`)
    holds a pointer to data owned by *another* Perl object's C counterpart
    (e.g., `upb_MessageDef` owned by `Protobuf::DescriptorPool`'s
    `upb_DefPool`), the descriptor object **MUST** hold a strong Perl reference
    to the pool object. Otherwise, the pool can be garbage collected, freeing
    the C memory and causing the descriptor to point to garbage.
    *   **Solution:** Store a reference to the owner object within the dependent
        object (e.g., `has '_pool' => (is => 'ro', required => 1);` in
        `Protobuf::Descriptor`).
*   **Arena Management:** When `upb` parses data (like `FileDescriptorSet`), the
    arena used for parsing must live as long as the definitions are needed by
    the `upb_DefPool`. Attaching the arena to the `Protobuf::DescriptorPool`
    object lifecycle is crucial.

### 2. XS Build & Compilation

*   **`PERL_NO_PPPORT`:** To reliably prevent `ppport.h` from being included and
    causing type/macro conflicts, add `-DPERL_NO_PPPORT` to the `DEFINE` option
    in `Makefile.PL`. Simply defining it in C files is not sufficient as it may
    not be active early enough during the inclusion of Perl's core headers.
*   **`make realclean` is Your Friend:** When .xs files change, especially
    function signatures or dependencies, `make clean` might not be enough. `make
    realclean` forces a more thorough cleanup, including `blib/` and the
    Makefile itself, ensuring a fresh build. Always re-run `perl Makefile.PL`
    after `make realclean`.
*   **Timestamps Matter:** `make` relies on timestamps. If a `.c` file generated
    from `.xs` isn't older than the `.xs` file, it might not recompile. `touch`
    can be used to update the `.xs` timestamp to force recompilation.
*   **XSUB Naming Collisions:** Be wary of naming conflicts between Perl
    subroutines defined in `.pm` files and XSUBs exposed from `.xs` files. If
    they have the same fully qualified name, the XSUB will typically overwrite
    the Perl sub at runtime.
    *   **Solution:** Name internal XSUBs with a leading underscore or use a
        different naming convention to disambiguate (e.g.,
        `_internal_xs_version`).
*   **Typemaps & XSUB Arguments:** When an XSUB signature in the `.xs` file
    includes typemapped C types (e.g., `upb_DefPool *`), you must pass a
    corresponding Perl variable of the type expected by the `INPUT` section of
    the typemap from the Perl caller. If the typemap expects to extract a C
    pointer from a Perl object, you must pass that Perl object.
*   **`dTHX` for C Functions:** Pure C functions not using the full XSUB macro
    boilerplate (like `MODULE = ...`) still need `dTHX;` at the start if they
    interact with the Perl API (e.g., `croak`, `newSViv`, `call_method`).
*   **`hv_delete_ent` Refcounting:** The `hv_delete_ent` function decrements the
    refcount of the key SV passed to it. Do not decref the lookup key SV again
    after this call.
*   **Object Cache Keys:** Use stringified pointer addresses as hash keys in the
    object cache for reliable lookups.
*   **Consolidate Core XS Helpers:** Common C helper functions for the XS layer
    (e.g., Arena wrappers, string utilities) should be placed in `xs/protobuf.c`
    and declared in `xs/protobuf.h` to ensure they are part of
    `libprotobuf_common.a` and available to C tests.

### 3. C Test Harnesses (`t/c/`)

*   **Embedding Perl for Tests:**

    *   Initialize with `PERL_SYS_INIT(&argc, &argv);`
    *   Call `perl_alloc()` then `perl_construct()`.
    *   `perl_parse()` is necessary. Critically, pass it `"-e", "0"` to prevent
        it from blocking on STDIN. Pass a pointer to an empty `xs_init` function
        as the second argument.
    *   **Do not call `PERL_SYS_TERM()`**, as this can cause mutex-related
        panics during shutdown in this context. `perl_destruct()` and
        `perl_free()` are sufficient.
    *   Example `main` in `t/c/my_test.c`:

        ```c
        #include "EXTERN.h"
        #include "perl.h"
        #include "xs/protobuf.h" // Your module's headers
        #include "t/c/upb-perl-test.h"     // Your C test helpers

        void xs_init(pTHX);

        int main(int argc, char** argv) {
            PERL_SYS_INIT(&argc, &argv);
            PerlInterpreter* my_perl = perl_alloc();
            perl_construct(my_perl);
            PL_exit_flags |= PERL_EXIT_DESTRUCT_END;

            char *embedding[] = { (char*)"", (char*)"-e", (char*)"0", NULL };
            perl_parse(my_perl, xs_init, 3, embedding, NULL);

            // Your tests here

            perl_destruct(my_perl);
            perl_free(my_perl);
            return 0;
        }

        void xs_init(pTHX) { /* Effectively empty */ }
        ```

*   **Makefile.PL for C Tests:** C tests in `t/c/` must be added to the
    `$c_test_config` in `MY::postamble` within `perl/Makefile.PL` to receive the
    correct compiler and linker flags, including those from
    `ExtUtils::Embed::ccopts()` and `ExtUtils::Embed::ldopts()`.

*   **Rebuilding C Tests:** `make` may not automatically recompile C tests when
    only `Makefile.PL` changes. To force a re-link, you may need to `rm -f
    t/c/your_test_binary` before running `make t/c/your_test_binary`.

### 4. Debugging XS

*   **`-g` Flag:** Ensure `-g` is added to `CCFLAGS` in `Makefile.PL` to include
    debug symbols in the compiled objects and shared libraries.
*   **GDB:** Running `gdb --args perl -Iblib/lib -Iblib/arch t/test_file.t` is
    essential for debugging SEGV's. Set breakpoints in your C/XS functions.
*   **Valgrind:** `Test::Valgrind` can catch memory errors, but output can be
    noisy with Perl & Moo internals. Focus on `InvalidRead`, `InvalidWrite`, and
    `DefinitelyLost` errors originating from your module's code.
*   **Diagnostic Prints:** `fprintf(stderr, ...)` in C/XS can be helpful, but
    ensure they flush (e.g., add `\n`).

### 4.1. Debugging by Instrumenting UPB

When a segmentation fault or inexplicable behavior occurs, and it's suspected to
originate from the interaction with the `upb` library itself, a powerful
debugging technique is to temporarily modify the `upb` source files (under
`@upb/`) by adding `fprintf(stderr, ...)` statements. Since we cannot
permanently modify the `upb` code, this is a transient debugging aid.

**Process:**

1.  **Identify Potential UPB Functions:** Based on the crash context, pinpoint
    the `upb` functions that are likely involved (e.g.,
    `upb_FieldDef_MessageSubDef`, functions within `def_pool.c`, etc.).
2.  **Add Debug Prints:** Insert `fprintf(stderr, "[DEBUG] UPB: %s:%d ...\n",
    __FILE__, __LINE__, ...)` calls within the `upb` source files to trace
    execution flow and inspect variable values.
3.  **Rebuild UPB:** Since the Perl module's build doesn't recompile `upb` from
    source, you need to trigger a rebuild of the `upb` components used by the
    Perl build. The exact command depends on the project setup, but it often
    involves a Bazel command:

    ```bash
    bazel build //upb/...
    ```

4.  **Rebuild Perl Module:** Clean and rebuild the Perl module to link against
    the newly instrumented `upb` code:

    ```bash
    cd perl
    make realclean
    perl Makefile.PL
    make -j$(nproc)
    ```

5.  **Run Tests:** Execute the C tests (`make test_c`) or Perl tests (`prove -bv
    t/some_test.t`) that were causing the issue.

6.  **Analyze Output:** Examine the STDERR output for the `[DEBUG] UPB:`
    messages to understand the state and flow within `upb` leading up to the
    crash.

7.  **Clean Up:** Once the issue is found, **it is crucial to revert the
    changes** made to the `@upb/` source files.

This method provides deep insights into the `upb` library's internal state,
which is invaluable for debugging complex crashes related to the C interface.

### 5. Moo and XS

*   **`BUILDARGS`:** To customize constructor argument processing in a Moo class
    with XS, define a C function (e.g., `Protobuf_Message_BUILDARGS`) and point
    to it from the `BUILDARGS` sub in your `.pm` file: `sub BUILDARGS {
    &Protobuf_Message_BUILDARGS }`.
*   **Attribute Initialization:** Lazy builders in Moo are a good way to defer
    XS calls for object attribute initialization until first use.
*   **`DEMOLISH`:** Be careful in `DEMOLISH` blocks to check for the existence
    and definedness of hash keys pointing to C pointers before attempting to
    free them, as the object might not have been fully constructed if `new()`
    croaked.

### 6. Segfault in t/c/convert/upb_to_sv.c (Dec 2025)

*   **Symptom:** Segfault at the very end of the `make test_c` run, after all
    test output from `t/c/convert/upb_to_sv` had been printed.
*   **Initial Misdiagnosis:** Believed to be an issue within the
    `PerlUpb_UpbToSv` function call for message types, as prints before the call
    worked, but not at the function entry.
*   **Debugging Steps:**
    1.  Extensive `fprintf` in `PerlUpb_UpbToSv` and the test runner.
    2.  Added ASan flags to `Makefile.PL` (`CCFLAGS` and `LDDLFLAGS`). ASan did
        not initially flag the root cause.
    3.  Instrumented the `set_message_val` function in
        `t/c/convert/types/message.c`.
    4.  Corrected message name from `test.NestedMessage` to
        `perltest.NestedMessage`.
    5.  Added more debug prints to the `PerlUpb_WrapMessage` stub.
    6.  Fixed `const` correctness in `PerlUpb_WrapMessage` signature.
    7.  Instrumented the `main` function's teardown sequence in
        `t/c/convert/upb_to_sv.c`.
*   **Root Cause:** A copy-paste error led to `upb_DefPool_Free(test_pool);`
    being called twice in `t/c/convert/upb_to_sv.c`'s `main` function. This
    double free corrupted memory, causing a segfault during the final stages of
    program exit.
*   **Lesson:** When segfaults occur at the end of a test, pay close attention
    to cleanup code in the test's `main` function. Double-free errors can
    manifest late. Also, meticulously check string literals used in lookups.

By keeping these points in mind, developing Perl XS extensions can be made more
robust and less error-prone.

--------------------------------------------------------------------------------

<!-- Section: core/c-perl-interface.md -->

## C-Perl Interface (XS)

The interface between Perl and the C `upb` library will be implemented using XS.

### Key Principles

1.  **Thin Wrappers:** Perl objects (e.g., `Protobuf::Descriptor`,
    `Protobuf::Message`) will be thin wrappers around the C `upb` structs
    (`upb_MessageDef`, `upb_Message`). The Perl object will primarily hold a
    pointer to the C struct.

2.  **Object Ownership:** The `upb` C objects are arena-allocated. The Perl
    wrapper objects do not *own* the C objects in terms of memory management.
    The `Protobuf::Arena` object wrapper will be responsible for freeing the
    underlying `upb_Arena`.

3.  **Typemaps:** Custom typemaps will be used to convert between C pointers and
    Perl objects. These typemaps will leverage the Object Caching mechanism to
    ensure the same Perl object is returned for the same C pointer.

    -   `T_PTROBJ_UPB`: A typemap entry for `upb` pointers that need to be
        cached and wrapped.

4.  **XS Functions:** XSUBs will:

    -   Accept Perl objects as arguments.
    -   Extract the underlying C pointer from the Perl object.
    -   Call the appropriate `upb` library functions.
    -   Wrap the returned `upb` pointers (if any) back into Perl objects using
        the caching mechanism.

### Example Workflow (Conceptual)

```c
// XS function to get a message's descriptor
SV* get_descriptor(pTHX_ Protobuf__Message self) {
    upb_Message *msg = INT2PTR(upb_Message*, SvIV(SvRV(self)));
    const upb_MessageDef *mdef = upb_Message_GetDef(msg);

    // This function will handle cache lookup or creation
    return protobuf_c_to_perl_obj(aTHX_ mdef, "Protobuf::Descriptor");
}
```

### Integration and Stability

To ensure world-class performance and stability in integrated environments
(e.g., Mojo, Coro), the interface follows these advanced principles:

1.  **Arena Sharing and Pinning**: When returning a sub-message or descriptor,
    the XS layer MUST ensure the child object pins the parent's memory by
    holding a strong reference to the parent's `arena_sv` or `pool_sv`. This
    ensures that the underlying `upb_Arena` remains valid even if the parent
    Perl object goes out of scope.
2.  **Cross-Interpreter Isolation**: Every `PerlInterpreter` instance (ithreads
    or workers) MUST have its own independent XS state. The object cache and any
    internal C-level registries are stored as member variables of the
    interpreter context (or via `PL_modglobal`) to prevent data corruption
    between threads.
3.  **Interrupt Resilience (Croak Safety)**: The interface is designed to
    maintain state integrity if a Perl `croak` or `die` (which uses `longjmp`)
    occurs during an operation. Resource management in C follows the
    "Allocate-Then-Register" pattern to ensure that any partially created
    objects are either properly tracked or safely leaked only until the arena is
    destroyed.
4.  **Lock-Free Multi-Access**: Read-only operations (like descriptor lookups)
    are architected to be lock-free, enabling high-performance concurrent access
    from multiple Perl coroutines.
5.  **Thread-Safe Global Freezing**: (Planned) Implement a mechanism to "freeze"
    a `DescriptorPool`, allowing it to be safely shared across multiple Perl
    interpreters (ithreads) without deep cloning, leveraging shared C-level
    pointers.
6.  **TSan-Verified Concurrency**: (Planned) The interface and underlying
    C-state management are verified using ThreadSanitizer (TSan) during
    continuous integration to guarantee race-free operation in multi-threaded
    environments.

--------------------------------------------------------------------------------
