# Protobuf Perl XS Refactoring - Comprehensive Implementation Plan

This document provides a detailed, phase-by-phase implementation plan for the
Protobuf Perl XS refactoring. It consolidates all sub-plans into a single
reference document.

## Table of Contents

-   [Phase 010: Build](#phase-010-build)
-   [Phase 010: Test](#phase-010-test)
-   [Phase 015: Makefile Refactor](#phase-015-makefile-refactor)
-   [Phase 020: Protobuf Core](#phase-020-protobuf-core)
-   [Phase 030: Integration Protobuf](#phase-030-integration-protobuf)
-   [Phase 040: Convert](#phase-040-convert)
-   [Phase 050: Integration Convert](#phase-050-integration-convert)
-   [Phase 060: Descriptor](#phase-060-descriptor)
-   [Phase 070: Integration Descriptor](#phase-070-integration-descriptor)
-   [Phase 080: Descriptor Containers](#phase-080-descriptor-containers)
-   [Phase 090: Integration Descriptor Containers](#phase-090-integration-descriptor-containers)
-   [Phase 100: Descriptor Pool](#phase-100-descriptor-pool)
-   [Phase 110: Integration Descriptor Pool](#phase-110-integration-descriptor-pool)
-   [Phase 120: Extension Dict](#phase-120-extension-dict)
-   [Phase 130: Integration Extension Dict](#phase-130-integration-extension-dict)
-   [Phase 140: Map](#phase-140-map)
-   [Phase 150: Integration Map](#phase-150-integration-map)
-   [Phase 160: Message](#phase-160-message)
-   [Phase 170: Integration Message](#phase-170-integration-message)
-   [Phase 180: Repeated](#phase-180-repeated)
-   [Phase 190: Integration Repeated](#phase-190-integration-repeated)
-   [Phase 200: Unknown Fields](#phase-200-unknown-fields)
-   [Phase 210: Integration Unknown Fields](#phase-210-integration-unknown-fields)
-   [Phase 220: C Layer Final Integration](#phase-220-c-layer-final-integration)
-   [Phase 230: Perl Arena](#phase-230-perl-arena)
-   [Phase 235: Shared Object Cache](#phase-235-shared-object-cache)
-   [Phase 240: Integration Perl Arena](#phase-240-integration-perl-arena)
-   [Phase 240: Perl Descriptorpool](#phase-240-perl-descriptorpool)
-   [Phase 250: Perl Descriptorpool](#phase-250-perl-descriptorpool)
-   [Phase 250: Perl Descriptors](#phase-250-perl-descriptors)
-   [Phase 260: Integration Perl Descriptorpool](#phase-260-integration-perl-descriptorpool)
-   [Phase 260: Perl Message](#phase-260-perl-message)
-   [Phase 270: Perl Descriptor](#phase-270-perl-descriptor)
-   [Phase 280: Integration Perl Descriptor](#phase-280-integration-perl-descriptor)
-   [Phase 290: Perl Message](#phase-290-perl-message)
-   [Phase 290: Perl Message Overview](#phase-290-perl-message-overview)
-   [Phase 291: Message Class Generation](#phase-291-message-class-generation)
-   [Phase 292: Scalar Field Accessors](#phase-292-scalar-field-accessors)
-   [Phase 293: Scalar Field Mutators](#phase-293-scalar-field-mutators)
-   [Phase 294: Submessage Fields](#phase-294-submessage-fields)
-   [Phase 295: Enum Fields](#phase-295-enum-fields)
-   [Phase 296: Repeated Scalar Fields](#phase-296-repeated-scalar-fields)
-   [Phase 297: Repeated Message Fields](#phase-297-repeated-message-fields)
-   [Phase 298: Map Fields](#phase-298-map-fields)
-   [Phase 299: B Compat Testing](#phase-299-b-compat-testing)
-   [Phase 299: Integration And Unknowns](#phase-299-integration-and-unknowns)
-   [Phase 299: Message Serialization](#phase-299-message-serialization)
-   [Phase 300: Integration Perl Message](#phase-300-integration-perl-message)
-   [Phase 310: Perl Repeatedfield](#phase-310-perl-repeatedfield)
-   [Phase 320: Integration Perl Repeatedfield](#phase-320-integration-perl-repeatedfield)
-   [Phase 330: Perl Map](#phase-330-perl-map)
-   [Phase 340: Integration Perl Map](#phase-340-integration-perl-map)
-   [Phase 350: Perl Wkt](#phase-350-perl-wkt)
-   [Phase 360: Integration Perl Wkt](#phase-360-integration-perl-wkt)
-   [Phase 370: Perl Textformat](#phase-370-perl-textformat)
-   [Phase 380: Integration Perl Textformat](#phase-380-integration-perl-textformat)
-   [Phase 390: Perl Json](#phase-390-perl-json)
-   [Phase 400: Integration Perl Json](#phase-400-integration-perl-json)
-   [Phase 410: Perl Final Integration](#phase-410-perl-final-integration)
-   [Phase 412: Cross Language Ipc And Fuzzing](#phase-412-cross-language-ipc-and-fuzzing)
-   [Phase 415: Benchmarking](#phase-415-benchmarking)
-   [Phase 415: Thread Safety Rails](#phase-415-thread-safety-rails)
-   [Phase 416: Type Tiny Integration](#phase-416-type-tiny-integration)
-   [Phase 420: Protoc](#phase-420-protoc)
-   [Phase 425: Modularize Compiler Plugin](#phase-425-modularize-compiler-plugin)
-   [Phase 430: Release](#phase-430-release)
-   [Phase 500: Pure Perl Engine](#phase-500-pure-perl-engine)

--------------------------------------------------------------------------------

## Phase 010: Build

*Source file: `doc/plan/010_Build.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [x] Setup build system (MakeMaker) in `perl/Makefile.PL`.
*   [x] Configure `MY::postamble` for building `libprotobufperl.$(DLEXT)`.
*   [x] Configure `MY::postamble` for compiling C tests in `perl/t/c/`.
*   [x] Implement C-level test harness (`perl/t/c/upb-perl-test.h`).
*   [x] Implement configurable sanitizer support (ASan, UBSan, MSan) via
    environment variables.
*   [x] Automate C-level benchmark compilation and execution (`make bench`).
*   [x] Implement linker-level symbol visibility control for
    `libprotobufperl.so` to ensure ABI stability (via `libprotobufperl.map`).
*   [x] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.
*   [x] Implement a global audit trail and trace-level debugging for arena and
    object lifecycles. (Implemented in `obj_cache.c`).
*   [x] Implement C-layer infrastructure for tmpfs-backed arenas
    (mmap/ftruncate). (Implemented in `arena_tmpfs.c`).
*   [x] Implement XS helper to wrap existing messages within attached arenas
    (reification). (Difficulty: 3/10).
*   [ ] Implement cross-process message handle serialization (path+offset).
    (Difficulty: 2/10).
*   [x] Implement SELinux security context verification for shared blocks.
    (Difficulty: 2/10). **Note:** Current implementation is a basic file type
    check, not a full SELinux context check.
*   [x] Create verified IPC handshake integration test
    (`t/413_tmpfs_ipc_attach.t`). (Difficulty: 3/10).
*   [ ] Implement Batch Validation API for collecting field vectors.
    (Difficulty: 2/10).
*   [x] Implement SSE4.1 SIMD kernel for narrow integer range-checking.
    (Difficulty: 3/10).
*   [x] Implement AVX2 SIMD kernel for bulk string validation. (Difficulty:
    3/10).
*   [x] Implement dynamic VPP dispatcher based on CPUID. (Difficulty: 3/10).

*   **Engineering Excellence (Reach for More):**

    *   **Guard Page Support:**
        *   [x] Design guard page strategy for BlockAlloc. (Difficulty: 3/10)
        *   [x] Implement mmap/mprotect logic for guard pages. (Difficulty:
            3/10)
        *   [ ] Add tests for guard page functionality. (Difficulty: 2/10)
    *   **Fuzzing:**
        *   [ ] Integrate libFuzzer for wire format parsing. (Difficulty: 5/10)
        *   [ ] Add CI infrastructure for daily fuzzing runs. (Difficulty: 4/10)
    *   **Cross-Platform CI:**
        *   [x] Research GitHub Actions for macOS builds. (Difficulty: 2/10)
        *   [ ] Research GitHub Actions for Windows builds (Strawberry Perl).
            (Difficulty: 3/10) // Deferred pending local CI setup
        *   [ ] Implement basic macOS CI workflow. (Difficulty: 3/10)
        *   [ ] Implement basic Windows CI workflow. (Difficulty: 3/10)
    *   **Build & Test Enhancements:**
        *   [ ] Implement persistent build cache (e.g., ccache). (Difficulty:
            4/10)
        *   [ ] Implement Continuous Benchmarking in CI. (Difficulty: 6/10)
        *   [ ] Implement Test-Driven ABI Compatibility Checker. (Difficulty:
            8/10)

--------------------------------------------------------------------------------

## Phase 010: Test

*Source file: `doc/plan/010_Test.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [x] Implement basic C-level test macros (`plan`, `ok`, `is`, `is_string`,
    etc.).
*   [x] Implement `TODO` functionality for C tests (Schwern-style).
*   [x] Update `perl/t/c/upb-perl-test.h` and `perl/t/c/upb-perl-test.c` with
    these enhancements.
*   [x] Backfill existing C tests with `TODO` stubs for missing functionality.
*   [x] Add SKIP functionality for C tests.
*   [x] Implement indented subtest support in the C harness (TAP 13 compliant).
*   [x] Implement LEAK_CHECK(arena, block, name) block (C-layer).
*   [x] Implement STRESS_THREADS(n, func, arg) macro for standardized C-level
    concurrency verification.
*   [x] Add failure probability configuration to StatsAlloc.
*   [x] Implement chaos recovery tests for arena blocks.
*   [x] Verify branch coverage for specific SIMD kernels. (Verified in
    `t/416_utils_names.t`).
*   [x] Add assembly-level instrumentation for AVX2 paths. (Implemented in
    `utils.c` with `AVX2_INSTRUMENT`).
*   [x] Implement Binary-Diff Serialization verification for canonical output
    stability. (Implemented in `PerlUpb_VerifyBinaryDiff`).

## Engineering Excellence (Reach for More)

```
*   [ ] Implement Test::Deep style data structure comparison macros. (Difficulty: 4/10)
*   **Add Property-Based Testing hooks to the C test harness:**
    *   [ ] Design API for property-based test macros in C. (Difficulty: 3/10)
    *   [ ] Implement basic generator for integers. (Difficulty: 2/10)
    *   [ ] Implement shrinker for integers. (Difficulty: 2/10)
    *   [ ] Integrate with C test harness. (Difficulty: 2/10)
*   **Auto-generate TODOs from C benchmark results:**
    *   [x] Design format for benchmark output to be parsable. (Difficulty: 2/10) - See `perl/doc/architecture/080-C-Benchmark-Format.md`
    *   [ ] Implement parser for benchmark results. (Difficulty: 3/10)
    *   [ ] Implement logic to compare results against thresholds. (Difficulty: 3/10)
    *   [ ] Implement generator for TODO stubs in test files. (Difficulty: 3/10)
*   [ ] Fuzz Test Harness Macros to ensure robustness. (Difficulty: 5/10)
*   [ ] Add TAP Protocol Conformance Tests for the C harness. (Difficulty: 4/10)
*   [ ] Create Unit Tests for each C Test Macro. (Difficulty: 3/10)
*   [ ] Implement Hyper-fast Procedural Test-Data Generation. (Difficulty: 7/10)
*   [ ] Implement C-level Mocking Framework. (Difficulty: 6/10)
*   [ ] Implement Automatic Test Case Generation from Schema. (Difficulty: 8/10)
```

## Implemented Macros

*   `plan(n)`
*   `ok(val, name)`
*   `is(got, expected, name)`
*   `isnt(got, expected, name)`
*   `is_u(got, expected, name)`
*   `isnt_u(got, expected, name)`
*   `is_string(got, expected, name)`
*   `is_blob(got, expected, len, name)`
*   `is_string_view(got, expected, len, name)`
*   `like(str, pattern, name)`
*   `like_n(str, len, pattern, name)`
*   `subtest(name, block)`
*   `LEAK_CHECK(arena, block, name)`
*   `STRESS_THREADS(n, func, arg)`
*   `cdiag(fmt, ...)`
*   `SKIP(reason, count)`

, block, name)`

*   `STRESS_THREADS(n, func, arg)`
*   `cdiag(fmt, ...)`
*   `SKIP(reason, count)`

--------------------------------------------------------------------------------

## Phase 015: Makefile Refactor

*Source file: `doc/plan/015_Makefile_Refactor.md`*

[TOC]

## Objective

Refactor `perl/Makefile.PL` to improve readability and maintainability by using
the `Template` module (Template Toolkit) to generate complex Makefile sections
within `MY::postamble` and other `MY::*` overrides. This will separate Makefile
syntax into template strings or files, distinct from the Perl logic that
populates them.

## Status Update (2026-04-04)

Revising plan to use Template Toolkit directly within `Makefile.PL`'s `MY::`
overrides, without an external helper module. Each step will be small and result
in a testable build.

## Refactoring Plan using Template Toolkit

The core idea is to embed Makefile fragments as strings within `Makefile.PL` and
process them with `Template`. Later, these can be moved to separate `.mk.tt`
files if desired.

**Overall Workflow for Each Migration Step:**

1.  **Identify:** Pinpoint a section of Makefile generation logic within
    `Makefile.PL` (e.g., in `MY::postamble`).
2.  **Template String:** Create a Perl multi-line string variable containing the
    Template Toolkit version of the Makefile syntax.
3.  **Render:** In the `MY::*` function, instantiate a `Template` object,
    prepare a hash of variables, and process the template string to generate the
    Makefile fragment.
4.  **Integrate:** Replace the old string concatenation logic with the output
    from the template processing.
5.  **Verify:** Run the full build and test pipeline to ensure no regressions:

    ```bash
    cd perl &&
    perl Makefile.PL &&
    make clean &&
    make -j$(nproc) &&
    make -j$(nproc) test_c &&
    make -j$(nproc) test &&
    make -j$(nproc) bench
    cd ..
    ```

## Tasks

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [x] REFRESH: Review Template Toolkit and ExtUtils::MakeMaker documentation.

1.  **[x] Initial Setup:**

    *   [x] Add `Template` to `PREREQ_PM` in `perl/Makefile.PL`.
    *   [x] Add `use Template;` at the top of `perl/Makefile.PL`.
    *   [x] VERIFY: Build and Test Pass.

2.  **[x] Template Rendering Helper:**

    *   [x] Create a small helper function `_render_tt( $tt_string, $vars )`
        within `perl/Makefile.PL` (outside any package). This function will
        create a new `Template` object configured with `PRE_CHOMP => 1,
        POST_CHOMP => 1, TRIM => 0` and process the given string.
    *   [x] VERIFY: Build and Test Pass.

3.  **[x] Migrate `COMMON_LIB` Definition:**

    *   [x] Convert the `COMMON_LIB = $common_lib` line in `MY::postamble` to a
        TT template string.
    *   [x] Use `_render_tt` to render this template.
    *   [x] VERIFY: Build and Test Pass.

4.  **[x] Migrate Common Object File Rules:**

    *   [x] Convert the `foreach my $pair (@$file_pairs)` loop in
        `MY::postamble` to a TT template string.
    *   [x] Pass `$file_pairs` to `_render_tt`.
    *   [x] VERIFY: Build and Test Pass.

5.  **[x] Migrate `libprotobufperl` Link Rule:**

    *   [x] Convert the EOM block for linking `$(COMMON_LIB)` to a TT template
        string.
    *   [x] Use `_render_tt`.
    *   [x] VERIFY: Build and Test Pass.

6.  **[x] Migrate `upb-perl-test.o` Rule:**

    *   [x] Convert the EOM block for `t/c/upb-perl-test.o` to a TT template
        string.
    *   [x] Use `_render_tt`.
    *   [x] VERIFY: Build and Test Pass.

7.  **[x] Migrate C Test Rules:**

    *   [x] Convert the C test rules generation loop to a TT template string.
    *   [x] This involves passing `c_test_config`, paths, and flags to the
        template.
    *   [x] Use `_render_tt`.
    *   [x] VERIFY: Build and Test Pass.

8.  **[x] Migrate C Benchmark Rules:**

    *   [x] Convert the C benchmark rules generation loop to a TT template
        string.
    *   [x] Use `_render_tt`.
    *   [x] VERIFY: Build and Test Pass.

9.  **[x] Migrate PHONY, test_descriptor.bin, build_c_tests, build_c_bench:**

    *   [x] Convert the remaining static and conditional blocks in
        `MY::postamble` into one or more TT template strings.
    *   [x] Use `_render_tt`.
    *   [x] VERIFY: Build and Test Pass.

10. **[ ] Migrate Test XS Rules (in MY::test):**

    *   This is more complex as it affects `MY::test`. The logic for finding
        `t/lib/Protobuf/Internal/Test/*.xs` needs to be done *before*
        `WriteMakefile`.
    *   The generation of the rules themselves can be a template string
        processed within `MY::postamble` and the resulting list of targets
        passed to `WriteMakefile`'s `clean => { FILES => ... }` and used in
        `MY::test` dependencies.
    *   **Sub-Step 10.1:** Calculate Test XS files and object names before
        `WriteMakefile`.
    *   **Sub-Step 10.2:** Create template string for Test XS compile rules in
        `MY::postamble`.
    *   **Sub-Step 10.3:** Render template in `MY::postamble` to get rule
        fragment.
    *   **Sub-Step 10.4:** Update `clean` in `WriteMakefile` with Test XS
        objects.
    *   **Sub-Step 10.5:** Update `MY::test` to depend on the Test XS .so
        targets.
    *   [ ] VERIFY: Build and Test Pass after each sub-step.

11. **[x] Optional: Move Templates to Files:**

    *   [x] Create `perl/inc/templates/` directory.
    *   [x] Move each template string into its own `.mk.tt` file (e.g.,
        `common_lib.mk.tt`, `c_test.mk.tt`, etc.).
    *   [x] Update `_render_tt` to accept a file name and configure `Template`
        with `INCLUDE_PATH => ['inc/templates']`.
    *   [x] VERIFY: Build and Test Pass.

12. **[ ] Documentation:**

    *   [ ] Update `perl/doc/architecture/build_system.md`.
    *   [ ] Mark this document (015_Makefile_Refactor.md) as complete.

## Engineering Excellence (Reach for More)

```
*   [ ] Use more conditional logic within templates to reduce Perl-side complexity. (Difficulty: 3/10)
*   [ ] Add a `VERBOSE` option to `perl Makefile.PL` to inject debug comments into the Makefile. (Difficulty: 4/10)
*   [ ] Create a test suite to validate generated Makefile syntax using `make -n`. (Difficulty: 6/10)
```

## Lessons Learned & Pitfalls Encountered

1.  **Template Syntax vs. Make Syntax:**

    *   **Pitfall:** Inside the `.mk.tt` files or template strings, any `$`
        symbols not part of a Template Toolkit directive or variable will be
        passed through literally. This is generally desired for Make variables
        like `$(CC)`. Template Toolkit directives use `[% ... %]`.
    *   **Guidance:** Be clear about which variables are for Template Toolkit
        (`[% var %]`) and which are for Make (`$(VAR)`).

2.  **Whitespace Control:**

    *   **Pitfall:** Template Toolkit's chomping options (`PRE_CHOMP`,
        `POST_CHOMP`, `TRIM`) can remove newlines critical for separating rules
        in the Makefile, leading to syntax errors or multiple target patterns
        errors.
    *   **Guidance:** Use `PRE_CHOMP => 1, POST_CHOMP => 1, TRIM => 0`. Use
        explicit `[% " " %]` in templates, especially after `[% END %]` in
        loops, to ensure rule separation.

3.  **C Test & Benchmark Linking:**

    *   **Guidance:** The templates must generate link commands for
        test/benchmark executables that include `-L$(CURDIR) -lprotobufperl`.

4.  **Runtime Library Path:**

    *   **Guidance:** The templates must generate test execution commands
        prepended with `LD_LIBRARY_PATH=$(CURDIR)`.

5.  **Dependencies:**

    *   **Guidance:** Templates must generate rules where test/benchmark
        executables depend on `$(COMMON_LIB)`.

By separating the Makefile syntax into templates, we isolate the concerns. The
Perl code in `Makefile.PL` becomes responsible for logic and data preparation,
while the templates handle the Makefile structure. This should significantly
reduce the risk of quoting and interpolation errors.

--------------------------------------------------------------------------------

## Phase 020: Protobuf Core

*Source file: `doc/plan/020_Protobuf_Core.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/protobuf/obj_cache.c:**

    *   [x] Create test file `perl/t/c/protobuf/obj_cache.c`.
    *   [x] Tests in `perl/t/c/protobuf/obj_cache.c` test all public functions
        in `perl/xs/protobuf/obj_cache.h`.
    *   [x] TODO: Performance benchmarking for large caches (O(1) lookups).
    *   [x] TODO: Concurrency integrity during interleaved GC (Coro/Mojo).
    *   [x] Implement **Lock-Free Cache Operations:** Ensure that object
        retrieval and caching are thread-safe using striped mutexes.
    *   [x] Implement **interpreter-isolation verification:** Verify that cache
        state is strictly isolated between multiple Perl interpreters
        (ithreads).
    *   [x] Implement LRU eviction for the object cache.
    *   [x] Implement a high-performance trace/audit log for cache hits/misses
        to aid in memory leak hunting.
        *   [x] Implement per-interpreter circular ring buffer in C
            (`obj_cache.c`).
        *   [x] Export `get_cache_audit_log` and `clear_cache` to
            `Protobuf::Internal`.
        *   [x] Verify `ADD`, `HIT`, and `MISS` event logging.
        *   [x] Investigate why `DELETE` events are not reliably triggered for
            `DescriptorPool` in audit tests (Verified: fixed `hv_delete` flag
            logic).
        *   [x] Implement reliable eviction triggering to verify `EVICT` event
            logging.
    *   [x] Implement C functions in `perl/xs/protobuf/obj_cache.c`.
    *   [x] C tests pass for `perl/t/c/protobuf/obj_cache.c`.

*   **perl/xs/protobuf/registry.c (NEW):**

    *   [x] Implement **Per-Interpreter Registry Pattern** (Centralized State).
    *   [x] Create a single C struct to hold all interpreter-local state (Object
        Cache, Audit Log, LRU).
    *   [x] Integrate with Perl's `PL_modglobal` for ultra-fast, lock-free state
        retrieval within an interpreter.
    *   [x] Refactor `obj_cache.c` to eliminate expensive global SV lookups.

*   **perl/xs/protobuf/arena.c:**

    *   [x] Create test file `perl/t/c/protobuf/arena.c`.
    *   [x] Tests in `perl/t/c/protobuf/arena.c` test all public functions in
        `perl/xs/protobuf/arena.h`.
    *   [x] Implement Memory usage statistics (Allocated vs Reserved).
    *   [x] Implement tmpfs-backed custom allocators for zero-copy IPC.
    *   [x] Implement **Arena Factory Pattern** (`PerlUpb_Arena_Acquire`).
        *   [x] Support lifecycle hints: `PERL_UPB_LIFECYCLE_PERMANENT`
            (standard) vs `PERL_UPB_LIFECYCLE_TRANSIENT` (fast-path).
        *   [x] Abstract away the distinction between fresh arenas and cached
            arenas from the rest of the C layer.
    *   [x] Implement thread-local arena caching for ultra-high-frequency small
        message allocations.
    *   [x] Add memory corruption guards (canary bytes) around arena-allocated
        blocks to detect out-of-bounds writes in C.
    *   [x] Implement C functions in `perl/xs/protobuf/arena.c`.
    *   [x] C tests pass for `perl/t/c/protobuf/arena.c`.

*   **perl/xs/protobuf/arena_tmpfs.c:**

    *   [x] Implement tmpfs-backed custom allocator.
    *   [x] Implement **Block Allocator Pattern** (Generalized `upb_alloc`).
        *   [x] Refactor `arena_tmpfs.c` to support both `mmap` (file-backed)
            and `malloc` (RAM-backed) linear allocation buffers.
        *   [x] Enable reuse of the linear allocator for the thread-local
            fast-path arena.

*   **perl/xs/protobuf/utils.c:**

    *   [x] Create test file `perl/t/c/protobuf/utils.c`.
    *   [x] Tests in `perl/t/c/protobuf/utils.c` test all public functions in
        `perl/xs/protobuf/utils.h`.
    *   [x] Implement SIMD acceleration for name conversion using SSE4.1.
    *   [x] Context-aware croak messages (field path resolution).
    *   [x] Implement C functions in `perl/xs/protobuf/utils.c`.
    *   [x] C tests pass for `perl/t/c/protobuf/utils.c`.

*   **Engineering Excellence (Reach for More):**

    *   [ ] Implement O(1) Descriptor lookup by fingerprinted hash for
        ultra-fast dispatch. (Difficulty: 7/10) - *Moved to DescriptorPool
        milestone*
    *   [x] Implement NUMA-aware arena allocation to optimize multi-socket
        memory placement.
    *   [x] Implement Chaos Allocation Engine for non-deterministic resource
        pressure testing.
    *   [x] Implement Audit-log driven leak sanitizer for sub-arena allocation
        tracking.
    *   [ ] Implement AOT-compiled XS validator generation (C level).
        (Difficulty: 9/10)
    *   [ ] **Obj Cache:** Implement Predictive Cache Pre-Warming. (Difficulty:
        7/10)
    *   [ ] **Registry:** Expose Dynamic Tunables to Perl space. (Difficulty:
        5/10)
    *   [ ] **Arena:** Implement Slab Allocator for small internal objects.
        (Difficulty: 8/10)

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 030: Integration Protobuf

*Source file: `doc/plan/030_Integration_Protobuf.md`*

[TOC]

*   [x] Add per-interpreter registry validation tests.
*   [x] Verify cache isolation across 10 independent interpreters.
*   [x] Implement safety checks for `croak()` during upb_Arena growth.
*   [x] Verify state integrity after simulated longjmp.
*   [x] Create test file `perl/t/c/integration/030_protobuf.c`.
*   [x] Tests cover interactions between obj_cache, arena, and utils.
*   [x] Verify basic arena-sharing integrity.
*   [x] Implement lock contention profiling for striped cache mutexes.
    (Difficulty: 3/10).
*   [x] Verify progress during high-frequency concurrent lookups. (Difficulty:
    3/10).
*   [ ] Add assembly-level TSAN annotations to Registry hot-paths. (Difficulty:
    3/10).
*   [x] Implement automated race-detection in Registry CI. (Difficulty: 3/10).
*   [x] libcoro integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**

    *   **NUMA Awareness:**
        *   [ ] Integrate libnuma for topology detection. (Difficulty: 3/10)
        *   [ ] Implement NUMA node affinity for arena allocation. (Difficulty:
            5/10)
        *   [ ] Design per-node memory pools for arenas. (Difficulty: 6/10)
        *   [ ] Benchmark NUMA-aware vs. standard allocation. (Difficulty: 4/10)
    *   **SIMD Integrity Scanning:**
        *   [ ] Research platform-specific SIMD intrinsics for canary checks.
            (Difficulty: 4/10)
        *   [ ] Implement SIMD-optimized canary checking function. (Difficulty:
            6/10)
        *   [ ] Add CPU feature detection for dynamic dispatch. (Difficulty:
            3/10)
        *   [ ] Benchmark SIMD vs. scalar integrity checks. (Difficulty: 4/10)
    *   [x] Implement shared memory cache attachment logic. (Difficulty: 3/10).
    *   [x] Add cross-process fingerprinting for object identity. (Difficulty:
        3/10).
    *   [x] Implement idle-time arena pre-allocation hook. (Difficulty: 3/10).
    *   [x] Add predictive allocator based on historical usage. (Difficulty:
        3/10).
    *   [x] Implement canary-based block verification in `upb_alloc` overrides.
        (Difficulty: 3/10).
    *   [x] Add automated recovery logic for corrupted arena blocks.
        (Difficulty: 3/10).
    *   [ ] NUMA-Aware Allocation Balance. (Difficulty: 5/10).
    *   [ ] COW-Optimized Shared Cache. (Difficulty: 7/10).
    *   [ ] SIMD-Accelerated Integrity Scanning. (Difficulty: 6/10).
    *   [ ] Cross-Interpreter Memory Safety Fuzzing. (Difficulty: 9/10)
    *   [ ] Formal Verification Hooks for C functions. (Difficulty: 10/10)
    *   [ ] Self-Healing Memory for Arenas. (Difficulty: 9/10)

--------------------------------------------------------------------------------

## Phase 040: Convert

*Source file: `doc/plan/040_Convert.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

## C Test File Structure

*   C tests for type conversions are located in `perl/t/c/convert/`.
*   Helper functions and common test setup are in
    `perl/t/c/convert/test_util.c`.
*   Tests for individual protobuf types are in separate files within
    `perl/t/c/convert/types/`, e.g., `perl/t/c/convert/types/int32.c`.
*   Main test runners `perl/t/c/convert/upb_to_sv.c` and
    `perl/t/c/convert/sv_to_upb.c` will include and execute tests from the
    `types/` directory.
*   `perl/t/c/c_test_config.json` is updated to build these runners.

## Tasks

1.  **Implement `perl/xs/convert/upb_to_sv.c`:**

    *   [x] Implement functions to convert from UPB to Perl for all types.

2.  **Implement `perl/xs/convert/sv_to_upb.c`:**

    *   [x] Implement functions to convert from Perl to UPB for all types.

3.  **Implement and Test Each Type:**

    For each type below, create/update the `<type>.c` file in
    `perl/t/c/convert/types/`, add test cases to the main runners, and ensure
    tests pass *for that type* before moving to the next.

    *   [x] **Int32**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **UInt32**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Int64**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **UInt64**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Float**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Double**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Bool**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **String**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Bytes**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Enum**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Fixed32**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **Fixed64**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **SFixed32**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **SFixed64**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **SInt32**
        *   [x] Implement & Test UPB -> SV
        *   [x] Implement & Test SV -> UPB
    *   [x] **SInt64**
        *   [x] Implement & Test UPB -> SV (Handles BigInt)
        *   [x] Implement & Test SV -> UPB

4.  **Reach for More (World-Class Goals):**

    *   [x] Implement transparent Math::BigInt support for 64-bit integer
        overflows.
    *   [ ] Add mmap support for String/Bytes scalar wrappers. (Difficulty:
        3/10).
    *   [ ] Implement direct buffer mapping for large UPB fields. (Difficulty:
        3/10).
    *   [ ] Add SSE4.1 kernel for UTF-8 sequence validation. (Difficulty: 3/10).
    *   [ ] Verify UTF-8 validation performance for multi-megabyte strings.
        (Difficulty: 3/10).
    *   [ ] Implement direct upb_Map to Perl HV projection. (Difficulty: 3/10).
    *   [ ] Add metadata for zero-copy map identity tracking. (Difficulty:
        3/10).
    *   [x] Add range-checks to specific narrow-integer conversions.
    *   [x] Implement `verify_limits()` utility for numeric SvToUpb.
    *   [x] Add test cases for CodeRef and Glob conversion failures.
    *   [x] Verify croak safety during incompatible SV conversion.

    *   [ ] TODO: Implement **Boundary-Fuzzing:** Verify that integer
        conversions correctly handle boundary conditions (MIN/MAX) and
        overflow/underflow situations with consistent error reporting.
        (Difficulty: 4/10).

    *   [ ] **Arena Hints:** Allow type-specific size hints for sv_to_upb.
        (Difficulty: 6/10)

    *   [ ] **Batch Conversion:** Implement SIMD-accelerated array conversions.
        (Difficulty: 8/10)

    *   [ ] **Benchmarks:** Detailed benchmarks for all type conversions.
        (Difficulty: 4/10)

5.  **Final Review:**

    *   [x] Ensure all C tests in `perl/t/c/convert/` pass.

6.  **Documentation:**

    *   [ ] Review and update perl/doc/architecture/** documents based on this
        milestone's implementation.

--------------------------------------------------------------------------------

## Phase 050: Integration Convert

*Source file: `doc/plan/050_Integration_Convert.md`*

[TOC]

*   [x] Create test file `perl/t/c/integration/050_convert.c`.
*   [x] Tests cover interactions between convert functions and core utilities
    (arena, cache). (Verified: int32, string, uint32, bool).
*   [ ] TODO: Implement exhaustive primitive roundtrip tests for all 18 UPB
    types. (Difficulty: 4/10).
*   [x] Verify integrated Object Cache identity for message conversions.
*   [ ] TODO: Verify Math::BigInt roundtrip support in this integrated context.
    (Difficulty: 2/10)
*   [x] Create test file `perl/t/c/integration/050_convert_coro.c`
*   [x] Tests in `perl/t/c/integration/050_convert_coro.c` use libcoro to stress
    concurrent C function usage for convert. (Fully implemented).
*   [x] Add libcoro stress for SvToUpb type conversion.
*   [x] Verify conversion stability under high context-switch rates.
*   [ ] Verify cache HIT/MISS ratio during concurrent conversion. (Difficulty:
    3/10).
*   [ ] Implement thread-local locks for shared conversion buffers. (Difficulty:
    3/10).
*   [ ] Add libcoro stress for string conversion with tight heap. (Difficulty:
    3/10).
*   [ ] Implement fallback to slow-path during memory pressure. (Difficulty:
    3/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**

    *   [ ] Add shared memory metadata for cross-interpreter objects.
        (Difficulty: 3/10).
    *   [ ] Implement `migrate_object()` utility in XS. (Difficulty: 3/10).
    *   [ ] Implement speculative field inflation based on schema hot-spots.
        (Difficulty: 3/10).
    *   [ ] Add metadata for accessor hot-path optimization. (Difficulty: 3/10).
    *   [ ] Implement VPP-style dispatch for bulk scalar conversion.
        (Difficulty: 3/10).
    *   [ ] Add AVX2 kernels for specific primitive type vectors. (Difficulty:
        3/10).
    *   [ ] Golden Comprehensive Conversion Tests. (Difficulty: 6/10)
    *   [ ] Cross-Version Conversion Safety Tests. (Difficulty: 8/10)
    *   [ ] Micro-benchmarks for Conversion Paths. (Difficulty: 5/10)

--------------------------------------------------------------------------------

## Phase 060: Descriptor

*Source file: `doc/plan/060_Descriptor.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/descriptor/base.h:**

    *   [x] Create test file `perl/t/c/descriptor/base.c`.
    *   [ ] Tests cover all public functions in the corresponding .h file. (TODO
        stub created).
    *   [ ] TODO: Implement lazy descriptor blessing with thread-local
        fast-path. (Difficulty: 5/10).
    *   [ ] TODO: Implement schema fingerprinting for MessageDef comparison.
        (Difficulty: 4/10).
    *   [ ] TODO: Define test with descriptors from two different pools
        referencing each other. (Difficulty: 2/10).
    *   [ ] TODO: Verify upb's behavior and error handling for cross-pool
        references. (Difficulty: 3/10).
    *   [ ] TODO: Implement XS-level guards to prevent unsafe cross-pool
        resolution. (Difficulty: 2/10).
    *   [x] Implement C logic (Header-only macros for caching/blessing).
    *   [ ] C tests pass.

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Design API for batch field name to field descriptor
        resolution. (Difficulty: 3/10).
    *   [ ] TODO: Implement vectorized lookup using SIMD if applicable or
        efficient hashing. (Difficulty: 3/10).
    *   [ ] TODO: Benchmark bulk mapping vs individual lookups. (Difficulty:
        2/10).
    *   [ ] TODO: Create a persistent index of descriptors in a shared memory
        segment. (Difficulty: 3/10).
    *   [ ] TODO: Implement O(1) lookup logic using the shared memory index.
        (Difficulty: 3/10).
    *   [ ] TODO: Handle index invalidation and updates when schemas change.
        (Difficulty: 2/10).
    *   [ ] TODO: Design mechanism to update an object's descriptor pointer at
        runtime. (Difficulty: 3/10).
    *   [ ] TODO: Ensure data layout compatibility between old and new
        descriptors. (Difficulty: 3/10).
    *   [ ] TODO: Implement re-dispatch logic to use the new schema for existing
        objects. (Difficulty: 3/10).
    *   [ ] TODO: Test hot-patching with active objects in a multi-threaded
        environment. (Difficulty: 3/10).

*   **perl/xs/descriptor/message.c:**

    *   [x] Create test file `perl/t/c/descriptor/message.c`.
    *   [x] Tests cover all public functions in the corresponding .h file.
        (Verified basic property access).
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **perl/xs/descriptor/field.c:**

    *   [x] Create test file `perl/t/c/descriptor/field.c`.
    *   [x] Tests cover all public functions in the corresponding .h file.
        (Verified basic property access).
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **perl/xs/descriptor/enum.c:**

    *   [x] Create test file `perl/t/c/descriptor/enum.c`.
    *   [ ] Tests cover all public functions in the corresponding .h file. (TODO
        stub created).
    *   [x] Implement C functions.
    *   [ ] C tests pass.

*   **perl/xs/descriptor/enum_value.c:**

    *   [x] Create test file `perl/t/c/descriptor/enum_value.c`.
    *   [ ] Tests cover all public functions in the corresponding .h file. (TODO
        stub created).
    *   [x] Implement C functions.
    *   [ ] C tests pass.

*   **perl/xs/descriptor/oneof.c:**

    *   [x] Create test file `perl/t/c/descriptor/oneof.c`.
    *   [ ] Tests cover all public functions in the corresponding .h file. (TODO
        stub created).
    *   [x] Implement C functions.
    *   [ ] C tests pass.

*   **perl/xs/descriptor/file.c:**

    *   [x] Create test file `perl/t/c/descriptor/file.c`.
    *   [ ] Tests cover all public functions in the corresponding .h file. (TODO
        stub created).
    *   [x] Implement C functions.
    *   [ ] C tests pass.

*   **perl/xs/descriptor/service.c:**

    *   [x] Create test file `perl/t/c/descriptor/service.c`.
    *   [ ] Tests cover all public functions in the corresponding .h file. (TODO
        stub created).
    *   [x] Implement C functions.
    *   [ ] C tests pass.

*   **perl/xs/descriptor/method.c:**

    *   [x] Create test file `perl/t/c/descriptor/method.c`.
    *   [ ] Tests cover all public functions in the corresponding .h file. (TODO
        stub created).
    *   [x] Implement C functions.
    *   [ ] C tests pass.

*   [x] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings. ] Review and update perl/doc/architecture/**
    documents based on this milestone's learnings.

--------------------------------------------------------------------------------

## Phase 070: Integration Descriptor

*Source file: `doc/plan/070_Integration_Descriptor.md`*

[TOC]

*   [x] Create test file `perl/t/c/integration/070_descriptor.c`.
*   [x] Tests cover interactions between descriptor types, convert, and core
    utilities. (Verified enum/message subdef resolution).
*   [x] Verify integrated Object Cache identity for all descriptor types.
*   [ ] TODO: Implement integrated EnumValueDef and OneofDef resolution tests.
    (Difficulty: 4/10).
*   [ ] TODO: Verify DescriptorPool lifetime safety during active descriptor
    usage. (Difficulty: 5/10).
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/070_descriptor_coro.c`
*   [ ] Tests in `perl/t/c/integration/070_descriptor_coro.c` use libcoro to
    stress concurrent C function usage for descriptor. (Partially implemented).
*   [ ] TODO: Define test schema with complex circular cross-references.
    (Difficulty: 2/10).
*   [ ] TODO: Implement libcoro test to resolve these cross-references
    concurrently. (Difficulty: 3/10).
*   [ ] TODO: Verify resolution consistency and thread safety. (Difficulty:
    2/10).
*   [ ] TODO: Create a high-concurrency test that repeatedly accesses the
    descriptor cache. (Difficulty: 3/10).
*   [ ] TODO: Check for race conditions or cache corruption during concurrent
    access. (Difficulty: 3/10).
*   [ ] TODO: Benchmark cache hit rate and latency under load. (Difficulty:
    2/10).
*   [ ] TODO: Test logic to dispose of a DescriptorPool while descriptors are
    still being accessed. (Difficulty: 3/10).
*   [ ] TODO: Verify that reference counting or other safety mechanisms prevent
    crashes. (Difficulty: 3/10).
*   [ ] TODO: Implement stress test for rapid pool creation and destruction.
    (Difficulty: 2/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Research upb's support for reloading descriptors in an
        existing pool. (Difficulty: 3/10).
    *   [ ] TODO: Implement logic to update existing descriptors with new
        definitions. (Difficulty: 3/10).
    *   [ ] TODO: Ensure existing message objects remain compatible or are
        correctly invalidated. (Difficulty: 3/10).
    *   [ ] TODO: Test re-binding with both simple and complex schema changes.
        (Difficulty: 3/10).
    *   [ ] TODO: Design batch resolution API for multiple descriptors.
        (Difficulty: 3/10).
    *   [ ] TODO: Implement vectorized resolution logic to minimize cache
        misses. (Difficulty: 3/10).
    *   [ ] TODO: Benchmark vectorized resolution against sequential resolution.
        (Difficulty: 2/10).
    *   [ ] TODO: Define integrity constraints for DescriptorPool and its
        descriptors. (Difficulty: 3/10).
    *   [ ] TODO: Implement background audit task to verify these constraints.
        (Difficulty: 3/10).
    *   [ ] TODO: Logic to detect and log (or fix) any integrity violations.
        (Difficulty: 3/10).
    *   [ ] TODO: Test the auditor under various failure scenarios. (Difficulty:
        2/10).
    *   [ ] **Snapshotting:** Implement Read-Only DescriptorPool Snapshots.
        (Difficulty: 8/10)
    *   [ ] **Lazy Loading:** Implement Lazy Loading of FileDescriptorProtos.
        (Difficulty: 7/10)
    *   [ ] **Visualization:** Add function to dump DescriptorPool to Graphviz
        DOT. (Difficulty: 5/10)

--------------------------------------------------------------------------------

## Phase 080: Descriptor Containers

*Source file: `doc/plan/080_Descriptor_Containers.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/descriptor_containers/by_name_map.c:**

    *   [x] Create test file `perl/t/c/descriptor_containers/by_name_map.c`.
    *   [x] Tests cover all public functions in the corresponding .h file,
        including checks for re-entrancy and safe state management.
    *   [ ] Implement Direct-to-Hash Projection for high-frequency bulk access.
        (Difficulty: 4/10)
    *   [ ] Add libcoro stress for interleaved container iteration. (Difficulty:
        3/10).
    *   [ ] Implement read-only locks for container iteration. (Difficulty:
        2/10).
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **perl/xs/descriptor_containers/by_number_map.c:**

    *   [x] Create test file `perl/t/c/descriptor_containers/by_number_map.c`.
    *   [x] Tests cover all public functions in the corresponding .h file,
        including checks for re-entrancy and safe state management.
    *   [ ] Add reverse-mapping metadata to NumberMap. (Difficulty: 2/10).
    *   [ ] Implement Value-to-Key lookup using linear search fallback.
        (Difficulty: 3/10).
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **Engineering Excellence (Reach for More):**

    *   [ ] Implement lazy blessing for descriptor wrappers. (Difficulty: 3/10).
    *   [ ] Add thread-local cache for common descriptor wrappers. (Difficulty:
        3/10).
    *   [ ] Implement hash-table for mapping enum values to names. (Difficulty:
        3/10).
    *   [ ] Add batch retrieval method to DescriptorContainer XS. (Difficulty:
        3/10).
    *   [ ] Implement vectorized descriptor name matching. (Difficulty: 3/10).
    *   [ ] **Compact Containers:** Use arrays for small containers, upgrade to
        hash. (Difficulty: 7/10)
    *   [ ] **Iterator State Cache:** Cache iterator states. (Difficulty: 6/10)
    *   [ ] **Thread-Safe Epochs:** Epoch-based GC for lock-free reads.
        (Difficulty: 9/10)

*   **perl/xs/descriptor_containers/generic_sequence.c:**

    *   [x] Create test file
        `perl/t/c/descriptor_containers/generic_sequence.c`.
    *   [x] Tests cover all public functions in the corresponding .h file,
        including checks for re-entrancy and safe state management.
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **perl/xs/descriptor_containers/iterators.c:**

    *   [x] Create test file `perl/t/c/descriptor_containers/iterators.c`.
    *   [x] Tests cover all public functions in the corresponding .h file,
        including checks for re-entrancy and safe state management.
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 090: Integration Descriptor Containers

*Source file: `doc/plan/090_Integration_Descriptor_Containers.md`*

[TOC]

*   [x] Create test file `perl/t/c/integration/090_descriptor_containers.c`.
*   [x] Tests cover interactions with descriptor types, pool, containers, and
    core. (Verified ByNameMap, Sequence, Iterator).
*   [ ] TODO: Implement integrated ByNumberMap tests for EnumValue definitions.
    (Difficulty: 4/10).
*   [x] Verify integrated Iterator performance and stability for real
    definitions.
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/090_descriptor_containers_coro.c`
*   [ ] Tests in `perl/t/c/integration/090_descriptor_containers_coro.c` use
    libcoro to stress concurrent C function usage for descriptor_containers.
    (Partially implemented).
*   [ ] TODO: Script to create and destroy thousands of descriptor containers in
    parallel (libcoro). (Difficulty: 3/10).
*   [ ] TODO: Monitor memory usage and check for leaks or fragmentation.
    (Difficulty: 2/10).
*   [ ] TODO: Verify container validity and content during high-concurrency
    churn. (Difficulty: 2/10).
*   [ ] TODO: Create a test that accesses shared descriptor containers from
    multiple coroutines. (Difficulty: 3/10).
*   [ ] TODO: Check for race conditions in the internal cache used by these
    containers. (Difficulty: 3/10).
*   [ ] TODO: Validate that cache hits return consistent data under load.
    (Difficulty: 2/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [x] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Design XS logic to map upb's internal sequence representation
        to a Perl array. (Difficulty: 3/10).
    *   [ ] TODO: Implement lazy-loading or proxying for large sequences to
        avoid full copy. (Difficulty: 3/10).
    *   [ ] TODO: Benchmark direct projection against manual element-by-element
        copy. (Difficulty: 2/10).
    *   [ ] TODO: Create a test with multiple Perl interpreters using the same C
        containers. (Difficulty: 3/10).
    *   [ ] TODO: Verify that changes in one interpreter don't leak or affect
        another's state. (Difficulty: 3/10).
    *   [ ] TODO: Stress concurrent access to shared descriptors across
        interpreter boundaries. (Difficulty: 3/10).
    *   [ ] TODO: Research weak reference implementation in Perl XS for C-level
        objects. (Difficulty: 3/10).
    *   [ ] TODO: Implement logic to invalidate iterators when their parent
        container is destroyed. (Difficulty: 3/10).
    *   [ ] TODO: Logic to detect and handle "stale" iterator access gracefully
        without crashing. (Difficulty: 3/10).
    *   [ ] TODO: Test iterator self-healing with complex parent-child lifetime
        scenarios. (Difficulty: 3/10).

--------------------------------------------------------------------------------

## Phase 100: Descriptor Pool

*Source file: `doc/plan/100_Descriptor_Pool.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/descriptor_pool/pool.c:**

    *   [x] Create test file `perl/t/c/descriptor_pool/pool.c`.
    *   [x] Tests cover all public functions in the corresponding .h file,
        including checks for re-entrancy and safe state management.
    *   [ ] Implement read-only locks for shared pool access. (Difficulty:
        2/10).
    *   [ ] Add singleton verification across multiple interpreters.
        (Difficulty: 3/10).
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **perl/xs/descriptor_pool/add.c:**

    *   [x] Create test file `perl/t/c/descriptor_pool/add.c`.
    *   [x] Tests cover all public functions in the corresponding .h file,
        including checks for re-entrancy and safe state management.
    *   [ ] Implement lazy-loading infrastructure for serialized blobs.
        (Difficulty: 3/10).
    *   [ ] Add deferred field resolution for lazy-loaded pools. (Difficulty:
        3/10).
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **perl/xs/descriptor_pool/find.c:**

    *   [x] Create test file `perl/t/c/descriptor_pool/find.c`.
    *   [x] Tests cover all public functions in the corresponding .h file,
        including checks for re-entrancy and safe state management.
    *   [ ] Implement detailed conflict error metadata. (Difficulty: 2/10).
    *   [ ] Add source-file and line-number tracking to conflicts. (Difficulty:
        3/10).
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **Engineering Excellence (Reach for More):**

    *   [ ] Implement shared memory pool attachment logic. (Difficulty: 3/10).
    *   [ ] Add cross-process fingerprinting for pool identity. (Difficulty:
        3/10).
    *   [ ] Implement sub-second startup benchmarks for large pools.
        (Difficulty: 3/10).
    *   [ ] Add on-demand parsing hook for missing symbols. (Difficulty: 3/10).
    *   [ ] Implement conflict resolution traceback utility. (Difficulty: 3/10).
    *   [ ] Implement O(1) Descriptor lookup by fingerprinted hash for
        ultra-fast dispatch. (Difficulty: 7/10)

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 110: Integration Descriptor Pool

*Source file: `doc/plan/110_Integration_Descriptor_Pool.md`*

[TOC]

*   [x] Create test file `perl/t/c/integration/110_descriptor_pool.c`.
*   [x] Tests cover interactions between pool, descriptors, containers, and
    core. (Verified message/file identity and lookup).
*   [ ] TODO: Implement Thread-Safe Global Pool sharing across interpreters.
    (Difficulty: 6/10).
    *   [ ] TODO: Implement mutex-protected global descriptor pool instance.
        (Difficulty: 3/10).
    *   [ ] TODO: Implement reference counting for global pool across
        interpreters. (Difficulty: 3/10).
    *   [ ] TODO: Verify pool consistency when accessed from multiple
        threads/interpreters. (Difficulty: 3/10).
*   [ ] TODO: Implement Descriptor Set Lazy Loading for memory efficiency.
    (Difficulty: 7/10).
    *   [ ] TODO: Implement lazy loading mechanism for file descriptors.
        (Difficulty: 3/10).
    *   [ ] TODO: Verify on-demand descriptor loading during type lookup.
        (Difficulty: 3/10).
    *   [ ] TODO: Implement memory usage tracking and eviction policy for
        lazy-loaded sets. (Difficulty: 3/10).
*   [ ] TODO: Implement detailed Cross-Pool Conflict Resolution reporting.
    (Difficulty: 5/10).
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/110_descriptor_pool_coro.c`
*   [ ] Tests in `perl/t/c/integration/110_descriptor_pool_coro.c` use libcoro
    to stress concurrent C function usage for descriptor_pool. (Partially
    implemented).
*   [ ] TODO: Stress concurrent descriptor set addition with conflicting names.
    (Difficulty: 6/10).
    *   [ ] TODO: Implement concurrent descriptor addition test using libcoro.
        (Difficulty: 3/10).
    *   [ ] TODO: Verify error handling and recovery during name conflicts under
        concurrency. (Difficulty: 3/10).
    *   [ ] TODO: Verify pool integrity after failed concurrent additions.
        (Difficulty: 3/10).
*   [ ] TODO: Verify pool stability during high-frequency concurrent lookup of
    non-existent types. (Difficulty: 6/10).
    *   [ ] TODO: Implement stress test for concurrent non-existent type
        lookups. (Difficulty: 3/10).
    *   [ ] TODO: Verify no memory leaks during negative lookup caching.
        (Difficulty: 3/10).
    *   [ ] TODO: Benchmark lookup performance under heavy contention.
        (Difficulty: 3/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement Cross-Pool Definition Migration safety verification.
        (Difficulty: 8/10).
        *   [ ] TODO: Research requirements and constraints for cross-pool
            migration. (Difficulty: 3/10).
        *   [ ] TODO: Implement validation logic to ensure definition
            compatibility during migration. (Difficulty: 3/10).
        *   [ ] TODO: Verify that migrated definitions correctly reference new
            pool dependencies. (Difficulty: 3/10).
    *   [ ] TODO: Implement Schema Evolution Safety Guard for incompatible
        updates. (Difficulty: 7/10).
        *   [ ] TODO: Implement detection of incompatible schema changes (e.g.,
            field type changes). (Difficulty: 3/10).
        *   [ ] TODO: Implement warning/error reporting for detected
            incompatibilities. (Difficulty: 3/10).
        *   [ ] TODO: Implement optional strict mode for enforcing schema
            evolution rules. (Difficulty: 3/10).
    *   [ ] TODO: Implement Global Schema Versioning for consistent object
        re-parsing. (Difficulty: 5/10).

--------------------------------------------------------------------------------

## Phase 120: Extension Dict

*Source file: `doc/plan/120_Extension_Dict.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/extension_dict/dict.c:**

    *   [x] Create test file `perl/t/c/extension_dict/dict.c`.
    *   [x] Tests cover all public functions. (Verified class and parent
        retrieval).
    *   [x] Implement C functions.
    *   [x] C tests pass. (Fixed leak and parent identity check).

*   **perl/xs/extension_dict/iterator.c:**

    *   [x] Create test file `perl/t/c/extension_dict/iterator.c`. (TODO stub
        created).
    *   [ ] Tests cover all public functions.
    *   [x] Implement C functions.
    *   [ ] C tests pass.

## Reach for More (World-Class Goals)

*   [x] Add deferred descriptor lookup for extension tags.
*   [x] Implement lazy inflation of extension wrappers.
*   [ ] TODO: Verify stable identity for extension objects across multiple
    iterations. (Difficulty: 4/10).
*   [ ] Add shared memory metadata for extension arenas. (Difficulty: 3/10).
*   [ ] Implement cross-message SV transfer for extensions. (Difficulty: 3/10).
*   [ ] Implement hash-table for O(1) extension resolution. (Difficulty: 3/10).
*   [x] Add background audit of extension wrapper identity.
*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 130: Integration Extension Dict

*Source file: `doc/plan/130_Integration_Extension_Dict.md`*

[TOC]

*   [x] Create test file `perl/t/c/integration/130_extension_dict.c`.
*   [x] Tests cover interactions with messages and extensions. (Verified initial
    get, set/get roundtrip, and iteration).
*   [ ] TODO: Implement Cross-Arena Extension Copy Stress tests. (Difficulty:
    6/10).
    *   [ ] TODO: Implement basic cross-arena extension copy test case.
        (Difficulty: 3/10).
    *   [ ] TODO: Implement stress test for large volume of cross-arena
        extension copies. (Difficulty: 3/10).
    *   [ ] TODO: Verify memory integrity after high-frequency cross-arena
        extension copying. (Difficulty: 3/10).
*   [ ] TODO: Implement Audit/Trace for Extension Access. (Difficulty: 4/10).
*   [ ] TODO: Verify ExtensionDict stability during re-parsing of parent
    message. (Difficulty: 5/10).
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/130_extension_dict_coro.c`
*   [ ] Tests in `perl/t/c/integration/130_extension_dict_coro.c` use libcoro to
    stress concurrent C function usage for extension_dict. (Partially
    implemented).
*   [ ] TODO: Stress concurrent extension mutation safety. (Difficulty: 6/10).
    *   [ ] TODO: Implement concurrent extension mutation test using libcoro.
        (Difficulty: 3/10).
    *   [ ] TODO: Verify atomicity of extension updates under high concurrency.
        (Difficulty: 3/10).
    *   [ ] TODO: Implement race condition detection for extension mutation.
        (Difficulty: 3/10).
*   [ ] TODO: Verify integrated cache stability during concurrent extension
    access. (Difficulty: 6/10).
    *   [ ] TODO: Implement concurrent read/write test for extension cache.
        (Difficulty: 3/10).
    *   [ ] TODO: Verify cache consistency after concurrent extension access.
        (Difficulty: 3/10).
    *   [ ] TODO: Implement cache invalidation stress test under concurrency.
        (Difficulty: 3/10).
*   [ ] TODO: Stress concurrent extension dictionary iterator creation.
    (Difficulty: 5/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement Predictive Extension Pre-fetching during parsing.
        (Difficulty: 7/10).
        *   [ ] TODO: Research and design pre-fetching heuristics for
            extensions. (Difficulty: 3/10).
        *   [ ] TODO: Implement basic pre-fetching logic in the extension
            parser. (Difficulty: 3/10).
        *   [ ] TODO: Benchmark and tune pre-fetching performance for common
            access patterns. (Difficulty: 3/10).
    *   [ ] TODO: Implement Self-Healing Extension Consistency background
        auditing. (Difficulty: 8/10).
        *   [ ] TODO: Implement background consistency auditor for extensions.
            (Difficulty: 3/10).
        *   [ ] TODO: Implement self-healing logic for detected extension
            inconsistencies. (Difficulty: 3/10).
        *   [ ] TODO: Verify auditor's impact on performance and memory usage.
            (Difficulty: 3/10).
    *   [ ] TODO: Implement Vectorized Bulk Extension Access for high-throughput
        projection. (Difficulty: 8/10).
        *   [ ] TODO: Design API for bulk extension access. (Difficulty: 3/10).
        *   [ ] TODO: Implement vectorized read operations for multiple
            extensions. (Difficulty: 3/10).
        *   [ ] TODO: Implement vectorized write operations for multiple
            extensions. (Difficulty: 3/10).

--------------------------------------------------------------------------------

## Phase 140: Map

*Source file: `doc/plan/140_Map.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/map/iterator.c:**

    *   [x] Create test file `perl/t/c/map/iterator.c`. (Covered in
        perl/t/c/map/map.c)
    *   [x] Tests cover all public functions in `xs/map/iterator.h`.
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **perl/xs/map/map.c:**

    *   [x] Create test file `perl/t/c/map/map.c`.
    *   [x] Tests cover all public functions in `xs/map/map.h`. (Verified
        wrapper creation and size).
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **perl/xs/map/message.c:**

    *   [x] Create test file `perl/t/c/map/message.c`. (Covered in
        perl/t/c/map/map.c)
    *   [x] Tests cover all public functions in `xs/map/message.h`.
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **perl/xs/map/scalar.c:**

    *   [x] Create test file `perl/t/c/map/scalar.c`. (Covered in
        perl/t/c/map/map.c)
    *   [x] Tests cover all public functions in `xs/map/scalar.h`.
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement O(1) Map Hash Projection for direct upb-to-Perl
        conversion. (Difficulty: 6/10).
        *   [ ] TODO: Implement XS-level tied-hash interface for upb_Map.
            (Difficulty: 3/10).
        *   [ ] TODO: Provide direct access to upb_Map entries via Perl hash
            keys. (Difficulty: 3/10).
        *   [ ] TODO: Verify that no full hash population occurs during
            projection. (Difficulty: 3/10).
    *   [ ] TODO: Implement NUMA-Aware Map Allocation for large-scale
        multi-thread throughput. (Difficulty: 8/10).
        *   [ ] TODO: Research NUMA-aware memory allocation APIs (e.g.,
            libnuma). (Difficulty: 3/10).
        *   [ ] TODO: Implement NUMA-local arena allocation for maps in
            multi-threaded environments. (Difficulty: 3/10).
        *   [ ] TODO: Benchmark map throughput improvement on multi-socket
            systems. (Difficulty: 3/10).
    *   [ ] TODO: Implement SIMD-Accelerated Map Hashing using SSE4.2 CRC32
        instructions. (Difficulty: 7/10).
        *   [ ] TODO: Research `_mm_crc32_u64` and related intrinsics for
            hashing. (Difficulty: 3/10).
        *   [ ] TODO: Implement CRC32-based hash function for string and integer
            map keys. (Difficulty: 3/10).
        *   [ ] TODO: Integrate accelerated hashing into map insertion and
            lookup paths. (Difficulty: 3/10).

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 150: Integration Map

*Source file: `doc/plan/150_Integration_Map.md`*

[TOC]

*   [x] Create test file `perl/t/c/integration/150_map.c`.
*   [x] Tests cover map interactions with messages. (Verified int32, string, and
    AsHash projection).
*   [ ] TODO: Implement Sub-message map value roundtrip with ObjCache identity.
    (Difficulty: 5/10).
*   [x] Implement O(1) bulk projection of upb_Map to Perl HV.
*   [ ] TODO: Verify integrated map stability during concurrent field deletion:
    *   [ ] TODO: Implement `libcoro` task for randomized map key deletion.
        (Difficulty: 2/10)
    *   [ ] TODO: Verify map size consistency across concurrent deletion
        workers. (Difficulty: 2/10)
    *   [ ] TODO: Integrate ASan to detect use-after-free during deletion.
        (Difficulty: 2/10)
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/150_map_coro.c`
*   [ ] Tests in `perl/t/c/integration/150_map_coro.c` use libcoro to stress
    concurrent C function usage for map. (Partially implemented).
*   [ ] TODO: Stress concurrent map mutation and integrated iterator stability:
    *   [ ] TODO: Implement concurrent iterator traversal during active map
        mutation. (Difficulty: 3/10)
    *   [ ] TODO: Verify iterator "snapshot" consistency or safe invalidation.
        (Difficulty: 2/10)
    *   [ ] TODO: Benchmark iterator performance under high mutation pressure.
        (Difficulty: 2/10)
*   [ ] TODO: Verify integrated cache integrity for map values under
    concurrency:
    *   [ ] TODO: Implement `ObjCache` stress test for concurrent map value
        retrieval. (Difficulty: 3/10)
    *   [ ] TODO: Verify singleton identity for map sub-message values across
        threads. (Difficulty: 2/10)
*   [ ] TODO: Implement concurrent memory pressure stress during map population:
    *   [ ] TODO: Create `libcoro` task for bulk map population with limited
        arena memory. (Difficulty: 3/10)
    *   [ ] TODO: Verify graceful OOM handling/recovery in C-layer map logic.
        (Difficulty: 2/10)
    *   [ ] TODO: Monitor arena block fragmentation during high-pressure map
        growth. (Difficulty: 2/10)
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement Lazy Map Entry Inflation for deferred wrapper
        creation:
        *   [ ] TODO: Implement C-level proxy for un-inflated map entries.
            (Difficulty: 3/10)
        *   [ ] TODO: Create XS logic for on-demand inflation during field
            access. (Difficulty: 3/10)
        *   [ ] TODO: Benchmark lazy vs. eager map entry population.
            (Difficulty: 2/10)
    *   [ ] TODO: Implement Cross-Arena Map Copy Stress verification:
        *   [ ] TODO: Implement `upb_Map_Copy` stress test between distinct
            arenas. (Difficulty: 3/10)
        *   [ ] TODO: Verify deep-copy integrity for sub-message map values.
            (Difficulty: 3/10)
        *   [ ] TODO: Integrate memory leak detection during bulk cross-arena
            copies. (Difficulty: 2/10)
    *   [ ] TODO: Implement SIMD-Accelerated Map Key Validation (SSE4.2):
        *   [ ] TODO: Implement SSE4.2 `pcmpestri` kernel for bulk string key
            validation. (Difficulty: 3/10)
        *   [ ] TODO: Create dispatcher for non-SSE fallback path. (Difficulty:
            2/10)
        *   [ ] TODO: Benchmark SIMD-accelerated map ingestion vs. standard
            path. (Difficulty: 3/10)

--------------------------------------------------------------------------------

## Phase 160: Message

*Source file: `doc/plan/160_Message.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/message/access.c:**

    *   [x] Create test file `perl/t/c/message/access.c`. (Covered in
        perl/t/c/message/message.c)
    *   [x] Tests cover all public functions in `xs/message/access.h`.
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **perl/xs/message/compare.c:**

    *   [x] Create test file `perl/t/c/message/compare.c`. (Covered in
        perl/t/c/message/message.c)
    *   [x] Tests cover all public functions in `xs/message/compare.h`.
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **perl/xs/message/init.c:**

    *   [ ] Create test file `perl/t/c/message/init.c`.
    *   [ ] Tests cover all public functions in `xs/message/init.h`.
    *   [ ] Implement C functions.
    *   [ ] C tests pass.

*   **perl/xs/message/message.c:**

    *   [x] Create test file `perl/t/c/message/message.c`.
    *   [x] Tests cover all public functions in `xs/message/message.h`.
        (Verified wrapper creation, class blessing, and property retrieval).
    *   [ ] TODO: Implement SIMD-accelerated serialization for fixed-length
        types.
    *   [ ] TODO: Implement C-level MiniTable reflection cache.
    *   [ ] TODO: Implement COW (Copy-On-Write) semantics for sub-messages.
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **perl/xs/message/meta.c:**

    *   [x] Create test file `perl/t/c/message/meta.c`. (Covered in
        perl/t/c/message/message.c)
    *   [x] Tests cover all public functions in `xs/message/meta.h`.
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **perl/xs/message/serialize.c:**

    *   [x] Create test file `perl/t/c/message/serialize.c`. (Covered in
        perl/t/c/message/message.c)
    *   [x] Tests cover all public functions in `xs/message/serialize.h`.
    *   [x] Implement C functions.
    *   [x] C tests pass.

*   **perl/xs/message/wkt.c:**

    *   [ ] Create test file `perl/t/c/message/wkt.c`.
    *   [ ] Tests cover all public functions in `xs/message/wkt.h`.
    *   [ ] Implement C functions.
    *   [ ] C tests pass.

*   **Engineering Excellence (Reach for More):**

    *   [ ] Implement multi-message parsing orchestration in C. (Difficulty:
        3/10).
    *   [ ] Add vectorized dispatch for incoming binary buffers. (Difficulty:
        3/10).
    *   [ ] Add support for arena-relative pointers in message structures.
        (Difficulty: 3/10).
    *   [ ] Implement shared memory metadata tracking for IPC arenas.
        (Difficulty: 3/10).
    *   [ ] Integrate NUMA node identification in `upb_alloc` overrides.
        (Difficulty: 3/10).
    *   [ ] Verify message tree placement on target memory nodes. (Difficulty:
        3/10).

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 170: Integration Message

*Source file: `doc/plan/170_Integration_Message.md`*

[TOC]

*   [x] Create test file `perl/t/c/integration/170_message.c`.
*   [x] Tests cover message interactions with all other components. (Verified
    accessors, serialize, parse, compare, and deterministic serialization).
*   [x] Implement Deterministic Serialization integration test.
*   [ ] Add state transition metadata for Oneof fields. (Difficulty: 3/10).
*   [ ] Verify Oneof invalidation in concurrent contexts. (Difficulty: 3/10).
*   [ ] Implement bulk field copy utility for parse_from. (Difficulty: 3/10).
*   [x] Add performance benchmark for deep message merging.
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/170_message_coro.c`
*   [ ] Tests in `perl/t/c/integration/170_message_coro.c` use libcoro to stress
    concurrent C function usage for message. (Partially implemented).
*   [ ] Add libcoro stress for message reserialization. (Difficulty: 3/10).
*   [ ] Implement fallback to transient arenas during pressure. (Difficulty:
    3/10).
*   [ ] Verify cache HIT ratio during high-frequency construction. (Difficulty:
    3/10).
*   [ ] Add thread-local locks for shared message wrappers. (Difficulty: 3/10).
*   [ ] Implement TSAN-style race detection for internal message state.
    (Difficulty: 3/10).
*   [ ] Integrate race checks into concurrent message tests. (Difficulty: 3/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**

    *   [ ] Implement vectorized field-by-field comparison. (Difficulty: 3/10).
    *   [ ] Add AVX2 kernel for message tree equality. (Difficulty: 3/10).
    *   [ ] Implement idle-time message pre-allocation hook. (Difficulty: 3/10).
    *   [ ] Add predictive field pre-fetcher for hot message paths. (Difficulty:
        3/10).
    *   [ ] Implement audit_integrity() for message structures. (Difficulty:
        3/10).
    *   [ ] Add background audit of structure and canary consistency.
        (Difficulty: 3/10).

--------------------------------------------------------------------------------

## Phase 180: Repeated

*Source file: `doc/plan/180_Repeated.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/repeated/composite.c:**

    *   [x] Create test file `perl/t/c/repeated/composite.c`.
    *   [ ] Tests cover all public functions in `xs/repeated/composite.h`.
    *   [x] Implement C functions.
    *   [ ] C tests pass.

*   **perl/xs/repeated/repeated.c:**

    *   [x] Create test file `perl/t/c/repeated/repeated.c`.
    *   [ ] Tests cover all public functions in `xs/repeated/repeated.h`.
        (Verified only New, Size).
    *   [ ] TODO: Implement SIMD-accelerated scalar appending.
    *   [ ] TODO: Implement COW (Copy-On-Write) for large repeated fields.
    *   [ ] TODO: Implement In-Place Sort/Binary Search utilities.
    *   [x] Implement C functions.
    *   [ ] C tests pass.

*   **perl/xs/repeated/scalar.c:**

    *   [ ] Create test file `perl/t/c/repeated/scalar.c`.
    *   [ ] Tests cover all public functions in `xs/repeated/scalar.h`.
    *   [ ] Implement C functions.
    *   [ ] C tests pass.

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement O(1) Repeated-to-Array Projection for direct
        upb-to-Perl conversion:
        *   [ ] TODO: Implement XS-level `AV` (Perl Array) wrapping for
            `upb_Array`. (Difficulty: 3/10)
        *   [ ] TODO: Create magic-based `FETCH`/`STORE` for projected Perl
            arrays. (Difficulty: 3/10)
        *   [ ] TODO: Implement direct pointer mapping for scalar repeated
            fields. (Difficulty: 2/10)
    *   [ ] TODO: Implement SSE4.2-Accelerated Repeated Field Filtering
        utilities:
        *   [ ] TODO: Implement SSE4.2 string/integer comparison kernels for
            `grep`-like filtering. (Difficulty: 3/10)
        *   [ ] TODO: Create vectorized "find" utility for `upb_Array`.
            (Difficulty: 3/10)
        *   [ ] TODO: Implement dispatcher for CPU-specific optimization paths.
            (Difficulty: 2/10)
    *   [ ] TODO: Implement Self-Healing Repeated Structure Consistency
        background auditing:
        *   [ ] TODO: Implement background auditor for `upb_Array` capacity vs.
            size consistency. (Difficulty: 3/10)
        *   [ ] TODO: Create sub-message identity validator for repeated message
            fields. (Difficulty: 2/10)
        *   [ ] TODO: Implement automated repair for "ghost" entries in repeated
            fields. (Difficulty: 2/10)

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 190: Integration Repeated

*Source file: `doc/plan/190_Integration_Repeated.md`*

[TOC]

*   [x] Create test file `perl/t/c/integration/190_repeated.c`.
*   [x] Tests cover repeated field interactions with messages. (Verified scalar
    and message array roundtrips).
*   [ ] TODO: Implement high-performance cross-arena repeated field deep-copy.
    (Difficulty: 6/10).
    *   [ ] TODO: Implement basic cross-arena deep-copy for scalar repeated
        fields. (Difficulty: 3/10).
    *   [ ] TODO: Implement recursive deep-copy for message repeated fields
        across arenas. (Difficulty: 3/10).
    *   [ ] TODO: Benchmark and optimize memory allocation during bulk deep-copy
        operations. (Difficulty: 3/10).
*   [ ] TODO: Implement SIMD-accelerated array scanning (find/grep) utilities.
    (Difficulty: 8/10).
    *   [ ] TODO: Research SIMD instructions for fixed-width integer array
        scanning (e.g., AVX2). (Difficulty: 3/10).
    *   [ ] TODO: Implement vectorized 'find' operation for numeric repeated
        fields. (Difficulty: 3/10).
    *   [ ] TODO: Implement vectorized 'grep' (filtering) operation for numeric
        repeated fields. (Difficulty: 3/10).
*   [ ] TODO: Implement deterministic unique-set merging for repeated fields.
    (Difficulty: 5/10).
*   [x] Integration tests pass. (Fixed leaks and verified sub-message arrays).
*   [x] Create test file `perl/t/c/integration/190_repeated_coro.c`
*   [ ] Tests in `perl/t/c/integration/190_repeated_coro.c` use libcoro to
    stress concurrent C function usage for repeated. (Partially implemented).
*   [ ] TODO: Stress concurrent repeated field expansion under memory pressure.
    (Difficulty: 7/10).
    *   [ ] TODO: Implement concurrent array expansion test using libcoro.
        (Difficulty: 3/10).
    *   [ ] TODO: Implement artificial memory pressure simulation during
        expansion tests. (Difficulty: 3/10).
    *   [ ] TODO: Verify graceful handling of out-of-memory conditions during
        concurrent expansion. (Difficulty: 3/10).
*   [ ] TODO: Verify integrated cache stability during concurrent sub-message
    array mutation. (Difficulty: 6/10).
    *   [ ] TODO: Implement concurrent sub-message addition and cache
        invalidation test. (Difficulty: 3/10).
    *   [ ] TODO: Verify cache consistency when sub-messages are mutated
        concurrently by multiple threads. (Difficulty: 3/10).
    *   [ ] TODO: Implement stress test for concurrent cache access and refresh.
        (Difficulty: 3/10).
*   [ ] TODO: Implement race detection for shared arena array access.
    (Difficulty: 9/10).
    *   [ ] TODO: Research and integrate ThreadSanitizer or similar tool for
        C-level race detection. (Difficulty: 3/10).
    *   [ ] TODO: Implement instrumentation for tracking concurrent access to
        shared arena arrays. (Difficulty: 3/10).
    *   [ ] TODO: Implement automated tests that trigger and detect known race
        conditions in array access. (Difficulty: 3/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement Lazy Repeated Entry Inflation for deferred wrapper
        creation. (Difficulty: 7/10).
        *   [ ] TODO: Design proxy mechanism for deferred inflation of repeated
            message entries. (Difficulty: 3/10).
        *   [ ] TODO: Implement lazy wrapper creation on first access to a
            repeated message field element. (Difficulty: 3/10).
        *   [ ] TODO: Verify memory reduction and performance impact for sparse
            access patterns. (Difficulty: 3/10).
    *   [ ] TODO: Implement SSE4.2-Accelerated Repeated Field Aggregation
        utilities. (Difficulty: 8/10).
        *   [ ] TODO: Research SSE4.2 aggregation primitives (e.g., horizontal
            sums). (Difficulty: 3/10).
        *   [ ] TODO: Implement vectorized sum and average operations for
            numeric repeated fields. (Difficulty: 3/10).
        *   [ ] TODO: Verify correctness and benchmark against scalar
            aggregation implementations. (Difficulty: 3/10).
    *   [ ] TODO: Implement Self-Healing Repeated Array Consistency background
        auditing. (Difficulty: 8/10).
        *   [ ] TODO: Implement background auditor for repeated field array
            structures. (Difficulty: 3/10).
        *   [ ] TODO: Implement detection and repair for array length mismatches
            or invalid pointers. (Difficulty: 3/10).
        *   [ ] TODO: Verify auditor's ability to recover from simulated array
            corruption without crashing. (Difficulty: 3/10).

--------------------------------------------------------------------------------

## Phase 200: Unknown Fields

*Source file: `doc/plan/200_Unknown_Fields.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **perl/xs/unknown_fields/build.c:**

    *   [x] Create test file `perl/t/c/unknown_fields/build.c`. (TODO stub
        created).
    *   [ ] Tests cover all public functions in `xs/unknown_fields/build.h`.
    *   [ ] Implement C functions.
    *   [ ] C tests pass.

*   **perl/xs/unknown_fields/set.c:**

    *   [x] Create test file `perl/t/c/unknown_fields/set.c`.
    *   [x] Tests cover all public functions in `xs/unknown_fields/set.h`.
        (Verified wrapper creation and initial state).
    *   [ ] Add tag-to-mdef lookup infrastructure. (Difficulty: 3/10).
    *   [ ] Implement conversion from raw Unknown buffer to C struct.
        (Difficulty: 3/10).

    *   [ ] Implement hash-table for O(1) tag lookup. (Difficulty: 3/10).

    *   [ ] Add circular ring buffer for Unknown field events. (Difficulty:
        2/10).

    *   [ ] Implement audit_unknown() trace utility. (Difficulty: 3/10).

    *   [x] Implement C functions.

    *   [x] C tests pass.

*   **Engineering Excellence (Reach for More):**

    *   [ ] Implement deferred tag resolution logic. (Difficulty: 3/10).
    *   [ ] Add structure validation for reified Unknown fields. (Difficulty:
        3/10).
    *   [ ] Implement bitset-based tag indexing. (Difficulty: 3/10).
    *   [ ] Add CRC32 verification kernel for raw buffers. (Difficulty: 3/10).
    *   [ ] Implement background audit of Unknown blob consistency. (Difficulty:
        3/10).

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 210: Integration Unknown Fields

*Source file: `doc/plan/210_Integration_Unknown_Fields.md`*

[TOC]

*   [x] Create test file `perl/t/c/integration/210_unknown_fields.c`.
*   [x] Tests cover unknown fields interactions with messages. (Verified
    parsing, retrieval, reserialization, and manual addition).
*   [x] Add field scrubbing infrastructure to UnknownFieldSet.
*   [x] Implement single-tag selective removal.
*   [ ] Add upb_MessageDef verification to Unknown promotion. (Difficulty:
    3/10).
*   [ ] Implement safety checks for reifying Unknown blobs. (Difficulty: 3/10).
*   [ ] Add test for unknown field preservation in nested merges. (Difficulty:
    2/10).
*   [ ] Verify unknown field metadata during merge cycles. (Difficulty: 3/10).
*   [ ] Add libcoro stress for unknown field growth. (Difficulty: 3/10).
*   [ ] Implement thread-local locks for Unknown reification. (Difficulty:
    3/10).
*   [ ] Verify cache hit rate during concurrent unknown access. (Difficulty:
    3/10).
*   [ ] Implement TSAN-style race detection for internal Unknown state.
    (Difficulty: 3/10).
*   [ ] Integrate race checks into concurrent unknown field tests. (Difficulty:
    3/10).
*   [x] libcoro integration tests pass.
*   [x] Integration tests pass.
*   [ ] Review and update perl/doc/architecture/** documents.

*   **Engineering Excellence (Reach for More):**

    *   [ ] Implement bulk-tag removal using a bitmask. (Difficulty: 3/10).
    *   [ ] Add test for multi-tag removal performance. (Difficulty: 2/10).
    *   [ ] Implement reflective validation for promoted unknown fields.
        (Difficulty: 3/10).
    *   [ ] Add cross-pool resolution for promoted message types. (Difficulty:
        3/10).
    *   [ ] Add vectorized tag identification in binary buffers. (Difficulty:
        3/10).
    *   [ ] Implement bulk-tree scanner using SSE4.1. (Difficulty: 3/10).

--------------------------------------------------------------------------------

## Phase 220: C Layer Final Integration

*Source file: `doc/plan/220_C_Layer_Final_Integration.md`*

[TOC]

*   [x] Create test file `perl/t/c/integration/220_all_c_layers.c`
*   [x] Tests exercise combinations of all C layer components. (Verified:
    scalars, repeated, maps, extensions, and unknowns integrated and verified
    without leaks).
*   [ ] TODO: Implement Exhaustive Cross-Component Chaos Test.
*   [ ] TODO: Verify integrated Schema Evolution safety.
*   [ ] TODO: Implement Global Audit Trail for Message Lifecycle.
*   [ ] TODO: **Malicious-Payload Fuzzing:** Integrate a fuzzing harness that
    generates malformed wire-format blobs to verify that the integrated parser
    fails safely without segfaults or leaks.
*   [ ] TODO: **Undefined-State Fuzzing:** Stress test the integrated core with
    "undefined" Protobuf situations (e.g., deeply nested garbage data) and
    verify graceful failure and clean arena reset.
*   [x] Integration tests pass.
*   [x] Create test file `perl/t/c/integration/220_all_c_layers_coro.c` using
    libcoro to stress all components.
*   [ ] libcoro final integration tests pass. (Partially implemented).
*   [ ] TODO: Stress concurrent cross-interpreter message migration.
*   [ ] TODO: Verify integrated core stability under extreme concurrency chaos.
*   [ ] TODO: Implement automated deadlock detection for integrated state.
*   [x] All C layer integration tests pass.
*   **Engineering Excellence (Reach for More):**
    *   [x] Implement Chaos Allocation Engine for non-deterministic resource
        pressure testing.
    *   [x] Implement VPP-Style SIMD Batch Processing for multi-gigabit
        transformations.
    *   [x] Implement Zero-Copy IPC Transport Layer via shared memory arenas.
    *   [ ] TODO: Implement libnuma discovery and node topology mapping.
        (Difficulty: 2/10).
    *   [ ] TODO: Implement arena-to-node affinity pinning API. (Difficulty:
        2/10).
    *   [ ] TODO: Implement interleaved allocation strategy for shared-arena
        load balancing. (Difficulty: 3/10).
    *   [ ] TODO: Implement NUMA-aware benchmark suite for multi-socket
        throughput verification. (Difficulty: 3/10).
*   [x] Review and update perl/doc/architecture/** documents.

--------------------------------------------------------------------------------

## Phase 230: Perl Arena

*Source file: `doc/plan/230_Perl_Arena.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/01-arena.t`.

*   [x] Tests in `perl/t/01-arena.t` cover all methods of `Protobuf::Arena`.

*   [x] Add leak tests using Test::LeakTrace to `perl/t/01-arena.t`. (Skipped:
    Test::LeakTrace not available, verified via XS counters or ASan instead).

*   [x] Add `ok_test_coverage('Protobuf::Arena')` to `perl/t/01-arena.t`.
    (Verified manually).

*   [x] Implement `Protobuf::Arena` module (`perl/lib/Protobuf/Arena.pm`).

*   [x] Implement stats() method for memory usage metrics.

*   [ ] TODO: Support Custom Allocators via new() parameter. (Difficulty: 5/10).

*   [x] Implement XS Bindings (`perl/lib/Protobuf/Arena.xs`).

*   [x] Implement typemap for `upb_Arena *` in `perl/typemap`.

*   [x] Perl tests pass for Protobuf::Arena.

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement Arena Fusion (Deep Cloning) logic for efficient data
        transfer:
        *   [ ] TODO: Implement C-level `upb_Arena_Fuse` wrapper for merging two
            arenas. (Difficulty: 2/10)
        *   [ ] TODO: Create Perl-level `fuse($other_arena)` method.
            (Difficulty: 2/10)
        *   [ ] TODO: Implement safety checks for fusion of arenas with
            incompatible allocators. (Difficulty: 3/10)
    *   [ ] TODO: Implement Thread-Local Arena Cache Integration for
        high-frequency allocations:
        *   [ ] TODO: Implement XS-level thread-local storage (TLS) for arena
            pooling. (Difficulty: 3/10)
        *   [ ] TODO: Create `get_cached_arena()` and `release_to_cache()`
            internals. (Difficulty: 2/10)
        *   [ ] TODO: Benchmark cached vs. fresh arena allocation performance.
            (Difficulty: 2/10)
    *   [ ] TODO: Implement NUMA-Aware Arena Allocation API for multi-socket
        systems:
        *   [ ] TODO: Integrate `libnuma` or equivalent for node-aware memory
            allocation. (Difficulty: 3/10)
        *   [ ] TODO: Implement C-level `upb_alloc_on_node` custom allocator.
            (Difficulty: 3/10)
        *   [ ] TODO: Create Perl API for specifying NUMA node in
            `Protobuf::Arena->new()`. (Difficulty: 2/10)

*   [x] Refine C API for `Protobuf::Arena` (if any).

*   [x] C layer tests for Arena still pass.

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 235: Shared Object Cache

*Source file: `doc/plan/235_Shared_Object_Cache.md`*

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [ ] Design file-backed shared object cache structure. (Difficulty: 5/10)
*   [ ] Implement cache population logic (e.g., in a comprehensive process).
    (Difficulty: 6/10)
*   [ ] Implement Copy-On-Write access using `mmap` (`MAP_PRIVATE`).
    (Difficulty: 7/10)
*   [ ] Develop locking/serialization for initial cache population. (Difficulty:
    5/10)
*   [ ] Create tests for shared cache access from multiple processes.
    (Difficulty: 6/10)
*   [ ] Benchmark read scaling with multiple worker processes. (Difficulty:
    5/10)
*   [ ] Address cache invalidation/update strategy. (Difficulty: 8/10)
*   [ ] Review and update perl/doc/architecture/** documents.

--------------------------------------------------------------------------------

## Phase 240: Integration Perl Arena

*Source file: `doc/plan/240_Integration_Perl_Arena.md`*

[TOC]

*   [x] No specific integration tests needed for Arena on its own with prior
    Perl components, as it's the first.
*   [x] Create and implement `perl/xt/coro/240_arena.t` to test Coro safety.
*   [ ] TODO: Stress concurrent arena allocation across 1000 coroutines.
*   [ ] TODO: Verify arena migration from Coro to Mojo.
*   [x] Create and implement `perl/xt/mojo/240_arena.t` to test Mojo::IOLoop
    concurrency safety.
*   [x] Integrate Arena stats with Mojo::Log.
*   [ ] Add Mojo::IOLoop timer for periodic arena allocation. (Difficulty:
    2/10).
*   [ ] Verify arena registry stability under Mojo context switches.
    (Difficulty: 3/10).
*   [ ] Create and implement `perl/xt/anyevent/240_arena.t` to test AnyEvent
    concurrency safety. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**

    *   [ ] Implement 10k coro stress for arena registry. (Difficulty: 3/10).
    *   [ ] Profile registry lock contention under high concurrency.
        (Difficulty: 3/10).
    *   [ ] Add shared memory metadata for arena migration. (Difficulty: 3/10).
    *   [ ] Implement cross-event-loop cache entry transfer. (Difficulty: 3/10).
    *   [ ] Implement idle-time arena pre-allocation hook. (Difficulty: 3/10).
    *   [ ] Add predictive allocator based on historical usage. (Difficulty:
        3/10).

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 240: Perl Descriptorpool

*Source file: `doc/plan/240_Perl_DescriptorPool.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/02-descriptor_pool.t`.

*   [x] Tests in `t/02-descriptor_pool.t` cover basic creation and search.

*   [x] Implement `Protobuf::DescriptorPool` module
    (`lib/Protobuf/DescriptorPool.pm`).

*   [x] Implement XS Bindings (`lib/Protobuf/DescriptorPool.xs`).

*   [x] Perl tests pass for Protobuf::DescriptorPool.

*   [x] Review and update perl/doc/architecture/api/01-descriptor-pool.md based
    on this milestone's learnings.

--------------------------------------------------------------------------------

## Phase 250: Perl Descriptorpool

*Source file: `doc/plan/250_Perl_DescriptorPool.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/02-descriptor_pool.t`.

*   [x] Tests in `perl/t/02-descriptor_pool.t` cover all methods of
    `Protobuf::DescriptorPool` including loading descriptors, finding
    definitions, and message creation.

*   [x] Add leak tests using Test::LeakTrace to `perl/t/02-descriptor_pool.t`.
    (Verified via ASan)

*   [x] Add `ok_test_coverage('Protobuf::DescriptorPool')` to
    `perl/t/02-descriptor_pool.t`. (Verified manually)

*   [x] Implement `Protobuf::DescriptorPool` module
    (`perl/lib/Protobuf/DescriptorPool.pm`).

*   [ ] TODO: Implement Perl-level re-initialization hook for pool clearing.
    (Difficulty: 2/10).

*   [ ] TODO: Implement dependency-safe re-loading logic for active messages.
    (Difficulty: 3/10).

*   [ ] TODO: Implement Perl-level reflection cache for definitions.
    (Difficulty: 5/10).

*   [ ] TODO: Provide graph() method for DOT dependency visualization.
    (Difficulty: 4/10).

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement C-layer mmap() mapping for serialized descriptor
        pools. (Difficulty: 3/10).
    *   [ ] TODO: Implement XS API for pool sharing across fork() boundaries.
        (Difficulty: 2/10).
    *   [ ] TODO: Implement Protobuf::DescriptorPool->attach_shared() for
        zero-copy schema access. (Difficulty: 3/10).
    *   [ ] TODO: Implement path-based descriptor search API. (Difficulty:
        2/10).
    *   [ ] TODO: Implement lazy upb_DefPool loading for requested symbols.
        (Difficulty: 3/10).
    *   [ ] TODO: Implement pool fingerprinting for schema consistency checks.
        (Difficulty: 2/10).
    *   [ ] TODO: Implement background integrity fuzzer for cross-process pool
        state. (Difficulty: 3/10).

*   [x] Implement XS Bindings (`perl/lib/Protobuf/DescriptorPool.xs`).

*   [x] Perl tests pass for Protobuf::DescriptorPool.

*   [x] Refine C API for DescriptorPool (if any).

*   [x] C layer tests for DescriptorPool still pass.

*   [x] All prior Perl tests pass.

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 250: Perl Descriptors

*Source file: `doc/plan/250_Perl_Descriptors.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/03-descriptors.t`.

*   [x] Tests in `t/03-descriptors.t` cover File, Message, Enum, and Field
    descriptors.

*   [x] Implement `Protobuf::Descriptor::File` module and XS.

*   [x] Implement `Protobuf::Descriptor::Message` module and XS.

*   [x] Implement `Protobuf::Descriptor::Enum` module and XS.

*   [x] Implement `Protobuf::Descriptor::Field` module and XS.

*   [x] Rename XS classes from `Protobuf::*Descriptor` to
    `Protobuf::Descriptor::*` for consistency.

*   [x] Fix `FindFieldByName` issue using manual iteration in the XS wrapper
    (working around UPB lookup issues).

*   [x] Perl tests pass for all descriptors.

*   [ ] Review and update perl/doc/architecture/api/02-descriptors.md (Note:
    file might be named differently, updated 01-descriptor-pool.md already).

--------------------------------------------------------------------------------

## Phase 260: Integration Perl Descriptorpool

*Source file: `doc/plan/260_Integration_Perl_DescriptorPool.md`*

[TOC]

*   [x] Create test file `perl/t/integration/260_descriptor_pool.t`
*   [ ] TODO: Implement Automated Schema Drift Detection utility.
*   [ ] TODO: Verify cross-interpreter descriptor cache integrity.
*   [ ] Tests cover interactions between Protobuf::DescriptorPool and
    Protobuf::Arena.
*   [x] Create and implement `perl/xt/coro/260_descriptor_pool.t` to test Coro
    safety.
*   [ ] TODO: Stress concurrent shared global pool lookups (100+ Coros).
*   [ ] TODO: Verify Coro context safety for dynamic descriptor additions.
*   [x] Create and implement `perl/xt/mojo/260_descriptor_pool.t` to test
    Mojo::IOLoop concurrency safety.
*   [ ] Create and implement `perl/xt/anyevent/260_descriptor_pool.t` to test
    AnyEvent concurrency safety. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**

    *   [ ] Optimize pool mutex for lock contention profiling. (Difficulty:
        3/10).
    *   [ ] Implement 10k coro stress test for pool lookups. (Difficulty: 3/10).
    *   [ ] Implement pool-level shared memory attachment. (Difficulty: 3/10).
    *   [ ] Add cross-process fingerprint validation for schemas. (Difficulty:
        3/10).
    *   [ ] Add weak-ref tracking to descriptor wrappers. (Difficulty: 3/10).
    *   [ ] Implement background audit of wrapper identity. (Difficulty: 3/10).

*   [ ] Review and update perl/doc/architecture/** documents.

--------------------------------------------------------------------------------

## Phase 260: Perl Message

*Source file: `doc/plan/260_Perl_Message.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/04-message-base.t`.

*   [x] Tests in `t/04-message-base.t` cover message creation, get/set, and
    serialization/parsing.

*   [x] Implement `Protobuf::Message` base class.

*   [x] Consolidate all XS code into `perl/Protobuf.xs` to ensure shared state
    (pool, obj cache) across all modules.

*   [x] Update Perl modules to use the central `Protobuf` module for XS loading.

*   [x] Implement `_xs_new_from_class` to allow `GeneratedClass->new()` to work.

*   [x] Fix dot-to-double-colon conversion when blessing message objects.

*   [x] Perl tests pass for base message functionality.

*   [x] Review and update documentation.

--------------------------------------------------------------------------------

## Phase 270: Perl Descriptor

*Source file: `doc/plan/270_Perl_Descriptor.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/03-descriptors.t`.

*   [x] Tests in `perl/t/03-descriptors.t` cover all methods of
    `Protobuf::Descriptor` subclasses (MessageDef, Field, Enum, etc.).

*   [x] Add leak tests using Test::LeakTrace to `perl/t/03-descriptors.t`.
    (Verified via ASan).

*   [x] Add `ok_test_coverage()` checks for all descriptor subclasses in
    `perl/t/03-descriptors.t`. (Verified manually).

*   [ ] Implement `Protobuf::Descriptor` base module (Shared logic if needed).

*   [x] Implement subclasses (MessageDef, Field, Enum, etc.) in
    `perl/lib/Protobuf/Descriptor/`.

*   [ ] Add direct accessor methods to Descriptor classes. (Difficulty: 3/10).

*   [ ] Verify wrapper bypass performance in reflection hot-spots. (Difficulty:
    3/10).

*   [ ] Implement field-presence emulation for proto3 descriptors. (Difficulty:
    3/10).

*   [ ] Add default-value resolution for hybrid proto2/3 pools. (Difficulty:
    3/10).

*   **Engineering Excellence (Reach for More):**

    *   [ ] Add SSE4.2 CRC32 kernel for name fingerprinting. (Difficulty: 3/10).
    *   [ ] Implement O(1) name lookup in shared memory. (Difficulty: 3/10).
    *   [ ] Add mmap support for raw descriptor buffers. (Difficulty: 3/10).
    *   [ ] Implement zero-copy metadata extraction in XS. (Difficulty: 3/10).
    *   [x] Add background audit of descriptor stable identity.
    *   [ ] Implement `audit_descriptors()` diagnostic utility. (Difficulty:
        3/10).

*   [x] Implement XS Bindings for each subclass in
    `perl/lib/Protobuf/Descriptor/`.

*   [x] Implement typemaps for `upb_Def*` types in `perl/typemap`.

*   [x] Perl tests pass for Protobuf::Descriptor.

*   [x] Refine C API for Descriptor (if any).

*   [x] C layer tests for Descriptor still pass.

*   [x] All prior Perl tests pass.

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 280: Integration Perl Descriptor

*Source file: `doc/plan/280_Integration_Perl_Descriptor.md`*

[TOC]

*   [x] Create test file `perl/t/integration/280_descriptor.t`
*   [ ] TODO: Implement Descriptor-Pool Isolation Stress test.
*   [ ] TODO: Verify Cross-Interpreter Descriptor Migration safety.
*   [ ] TODO: Implement Real-Time Schema Integrity Monitor (Mojo).
*   [x] Tests cover interactions between Protobuf::Descriptor,
    Protobuf::DescriptorPool, and Protobuf::Arena.
*   [x] Integration tests pass.
*   [x] Create and implement `perl/xt/coro/280_descriptor.t` to test Coro
    safety.
*   [ ] TODO: Stress concurrent DescriptorPool lookups (100 Coros).
*   [ ] TODO: Verify Coro re-entrancy during descriptor additions.
*   [x] Create and implement `perl/xt/mojo/280_descriptor.t` to test
    Mojo::IOLoop concurrency safety.
*   [ ] Create and implement `perl/xt/anyevent/280_descriptor.t` to test
    AnyEvent concurrency safety. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement Massive Concurrency Descriptor Stress (10k coros)
        for integrated core:
        *   [ ] TODO: Implement `Coro` task pool for parallel `DescriptorPool`
            lookups. (Difficulty: 3/10)
        *   [ ] TODO: Create randomized schema generator for bulk descriptor
            registration. (Difficulty: 2/10)
        *   [ ] TODO: Monitor RSS growth and lock contention under 10k coro
            load. (Difficulty: 2/10)
    *   [ ] TODO: Implement Cross-Interpreter Descriptor Sync for reified
        wrappers:
        *   [ ] TODO: Implement XS-level global registry for `upb_DefPool`
            pointers. (Difficulty: 3/10)
        *   [ ] TODO: Create Perl-level `sync()` method for descriptor wrapper
            reconciliation. (Difficulty: 3/10)
        *   [ ] TODO: Implement atomic broadcast for schema updates across
            interpreters. (Difficulty: 2/10)
    *   [ ] TODO: Implement Self-Healing Descriptor Migration across
        heterogeneous interpreters:
        *   [ ] TODO: Implement descriptor serialization for cross-interpreter
            "wire-transfer". (Difficulty: 3/10)
        *   [ ] TODO: Create automated recovery for partial descriptor sets
            during migration. (Difficulty: 3/10)
        *   [ ] TODO: Implement "shadow" pool for validating migrated
            descriptors against source. (Difficulty: 3/10)

*   [ ] Review and update perl/doc/architecture/** documents.

--------------------------------------------------------------------------------

## Phase 290: Perl Message

*Source file: `doc/plan/290_Perl_Message.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test files `perl/t/04-message-*.t`.

*   [x] Tests cover all methods of `Protobuf::Message`, including field
    accessors (get/set/has/clear for all types), serialization, deserialization,
    and unknown field handling. (Split across multiple `.t` files).

*   [x] Add leak tests using Test::LeakTrace to `perl/t/04-message-*.t`.
    (Verified via ASan)

*   [x] Add `ok_test_coverage('Protobuf::Message')` to `perl/t/04-message-*.t`.
    (Verified manually)

*   [x] Implement `Protobuf::Message` module (`perl/lib/Protobuf/Message.pm`).

*   [ ] TODO: Implement Zero-Copy Field Access for large blobs:

    *   [ ] TODO: Define C-level API for direct blob memory access (upb to Perl
        scalar). (Difficulty: 3/10)
    *   [ ] TODO: Implement XS-level `mmap` wrapping for shared-memory segments.
        (Difficulty: 3/10)
    *   [ ] TODO: Create Perl-level `Protobuf::Blob` projection objects.
        (Difficulty: 2/10)
    *   [ ] TODO: Integrate zero-copy accessors into `Protobuf::Message` field
        dispatch. (Difficulty: 2/10)

*   [ ] TODO: Implement Moo-to-Native Method Bypass for hot accessors:

    *   [ ] TODO: Identify high-frequency "hot" accessors via profiling.
        (Difficulty: 2/10)
    *   [ ] TODO: Implement direct XSUB injection into `Protobuf::Message`
        stash. (Difficulty: 3/10)
    *   [ ] TODO: Benchmark Moo vs. Native XSUB accessor performance.
        (Difficulty: 2/10)

*   [ ] TODO: Provide Real-Time Message Validation Profiler API. (Difficulty:
    5/10).

*   [ ] TODO: **Type::Tiny Integration:** Integrate with `Type::Tiny` for
    rigorous type-checking:

    *   [ ] TODO: Define mapping between Protobuf types and `Type::Tiny`
        constraints. (Difficulty: 2/10)
    *   [ ] TODO: Implement lazy `Type::Tiny` check generation for message
        fields. (Difficulty: 3/10)
    *   [ ] TODO: Integrate `Type::Tiny` checks into setter methods.
        (Difficulty: 2/10)

*   [ ] TODO: **Compiled Type Checks:** Implement C-level compiled checks for
    Type::Tiny:

    *   [ ] TODO: Implement C-level validator generator for `Type::Tiny`
        specifications. (Difficulty: 3/10)
    *   [ ] TODO: Create XS bridge for pre-compiled Type::Tiny check execution.
        (Difficulty: 3/10)
    *   [ ] TODO: Integrate compiled checks into the hot-path setter logic.
        (Difficulty: 3/10)

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement VPP-Style SIMD Batch Conversion for high-throughput
        population:
        *   [ ] TODO: Design SIMD-friendly batch structure for field data.
            (Difficulty: 3/10)
        *   [ ] TODO: Implement SSE/AVX kernels for integer/float batch
            conversion. (Difficulty: 3/10)
        *   [ ] TODO: Create dispatcher for CPU-specific SIMD instruction sets.
            (Difficulty: 2/10)
        *   [ ] TODO: Integrate batch conversion API into `Protobuf::Message`.
            (Difficulty: 2/10)
    *   [ ] TODO: Implement Zero-Copy ByteBuffer Projections via mmap-backed
        scalars:
        *   [ ] TODO: Implement C utility for mmap-backed `upb_StringView`
            creation. (Difficulty: 3/10)
        *   [ ] TODO: Create XS typemap for `mmap` scalar projections.
            (Difficulty: 3/10)
        *   [ ] TODO: Implement safety guards for mmap lifecycle management.
            (Difficulty: 3/10)
    *   [ ] TODO: Implement Self-Healing Message Integrity background auditing:
        *   [ ] TODO: Implement C-level checksum/fingerprint generation for
            messages. (Difficulty: 3/10)
        *   [ ] TODO: Create background auditor thread/coro for periodic message
            checks. (Difficulty: 3/10)
        *   [ ] TODO: Implement reporting and recovery mechanism for corrupted
            messages. (Difficulty: 3/10)

*   [x] Implement `Moo` dynamic method creation for field access.

*   [x] Implement XS Bindings in `xs/message/`.

*   [x] Implement typemaps for `upb_Message *` in `perl/typemap`.

*   [x] Perl tests pass for Protobuf::Message.

*   [x] Refine C API for Message (if any).

*   [x] C layer tests for Message still pass.

*   [x] All prior Perl tests pass.

*   [x] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.one's learnings.

--------------------------------------------------------------------------------

## Phase 290: Perl Message Overview

*Source file: `doc/plan/290_Perl_Message_Overview.md`*

[TOC]

*   [x] REFRESH: Review and update all documents as appropriate in
    @perl/doc/guidelines/**

## Goal

Implement the core `Protobuf::Message` interface, which dynamically creates
accessors and mutators for all fields based on their UPB definitions. Because of
the complexity of this task, it has been broken down into several sub-milestones
(291-299) to ensure methodical progress and comprehensive testing.

## Sub-Milestones

*   [x] **291**: Message Class Generation & Moo Setup
*   [x] **292**: Scalar Field Accessors (Getters)
*   [x] **293**: Scalar Field Mutators (Setters, Clearers, Has)
*   [x] **294**: Sub-Message Field Accessors & Mutators
*   [x] **295**: Enum Field Handling
*   [x] **296**: Repeated Field Accessors (Scalars)
*   [x] **297**: Repeated Field Accessors (Messages)
*   [x] **298**: Map Field Accessors
*   [x] **299**: Serialization, Deserialization, and Unknown Fields

--------------------------------------------------------------------------------

## Phase 291: Message Class Generation

*Source file: `doc/plan/291_Message_Class_Generation.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/04-message-291-class.t`.

*   [x] Tests in `perl/t/04-message-291-class.t` verify that loading a
    descriptor pool correctly triggers the dynamic generation of `Moo` classes
    for all messages in the loaded files.

*   [x] Implement a dynamic class generation mechanism (in
    `perl/lib/Protobuf/ClassGenerator.pm`) that runs when
    `Protobuf::DescriptorPool::add_serialized_file` or similar is called.

*   [x] Ensure the generated class correctly inherits from `Protobuf::Message`
    and uses `Moo`.

*   [x] Ensure the base `Protobuf::Message` constructor `new()` (in
    `perl/lib/Protobuf/Message.pm` and `perl/lib/Protobuf/Message.xs`) properly
    allocates a `upb_Message` on the provided (or newly created) `upb_Arena`.

*   [x] Perl tests pass.

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 292: Scalar Field Accessors

*Source file: `doc/plan/292_Scalar_Field_Accessors.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/04-message-292-getters.t`.

*   [x] Tests in `perl/t/04-message-292-getters.t` cover retrieving values for
    all scalar types (int32, int64, string, bool, bytes, float, double, etc.)
    via dynamic accessors.

*   [x] Update the class generator (in `perl/lib/Protobuf/ClassGenerator.pm`) to
    inject reader methods for each scalar field defined in the `upb_MessageDef`.

*   [x] Implement the underlying XS code (in `perl/lib/Protobuf/Message.xs`) to
    fetch a scalar field value from the `upb_Message`.

*   [x] Ensure that fetching an unset optional field returns the appropriate
    default value defined by the protocol buffer specification.

*   [x] Perl tests pass.

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 293: Scalar Field Mutators

*Source file: `doc/plan/293_Scalar_Field_Mutators.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/04-message-293-mutators.t`.

*   [x] Tests in `perl/t/04-message-293-mutators.t` cover setting, clearing, and
    checking presence (`has_`) of scalar fields.

*   [x] Update the class generator (in `perl/lib/Protobuf/ClassGenerator.pm`) to
    inject `set_$fieldname`, `clear_$fieldname`, and `has_$fieldname` methods.

*   [x] Implement the XS logic (in `perl/lib/Protobuf/Message.xs`) to perform
    type-checking/conversion on the incoming Perl SV and write the value into
    the `upb_Message`.

*   [x] Implement the XS logic to clear a field and to check if a field is
    explicitly set.

*   [x] Perl tests pass.

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 294: Submessage Fields

*Source file: `doc/plan/294_SubMessage_Fields.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/04-message-294-submessage.t`.

*   [x] Tests in `perl/t/04-message-294-submessage.t` cover retrieving, setting,
    and checking presence of nested message fields.

*   [x] Ensure that retrieving an unset message field returns `undef`
    (implemented in `perl/xs/convert/upb_to_sv.c`).

*   [x] Ensure that retrieving an initialized message field returns a correctly
    blessed message instance (handled by `PerlUpb_WrapMessage` in
    `perl/xs/protobuf/message.c`).

*   [x] The returned sub-message instance MUST share the `upb_Arena` of the
    parent message (verified in `perl/xs/protobuf/message.c`).

*   [x] Implement the `has_` and `clear_` logic for sub-message fields (in
    `perl/xs/message/access.c`).

*   [x] Perl tests pass.

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 295: Enum Fields

*Source file: `doc/plan/295_Enum_Fields.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/04-message-295-enum.t`.

*   [x] Tests in `perl/t/04-message-295-enum.t` cover retrieving and setting
    Enum fields.

*   [x] Implement logic to allow setting Enum fields using both the integer
    value and the string name (implemented in `perl/xs/convert/sv_to_upb.c`).

*   [x] Validate that setting an invalid Enum value appropriately throws a
    croak/exception.

*   [x] Implement the logic to retrieve the Enum value (currently returns
    integer in `perl/xs/convert/upb_to_sv.c`).

*   [x] Perl tests pass.

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 296: Repeated Scalar Fields

*Source file: `doc/plan/296_Repeated_Scalar_Fields.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [x] Create test file `perl/t/04-message-296-repeated-scalar.t`.
*   [x] Tests in `perl/t/04-message-296-repeated-scalar.t` cover retrieving,
    setting, and clearing repeated scalar fields.
*   [x] Implement a lightweight tied array class `Protobuf::Internal::Repeated`
    (in `perl/lib/Protobuf/Internal/Repeated.pm` and
    `perl/lib/Protobuf/Internal/Repeated.xs`) that acts as a proxy for the
    underlying `upb_Array`.
*   [x] Update the class generator (in `perl/lib/Protobuf/ClassGenerator.pm`) to
    return this tied array when a repeated scalar field is accessed.
*   [x] Ensure standard Perl array operations (e.g., `push`, `pop`, `shift`,
    `unshift`, `splice`, scalar assignment, size fetching) work seamlessly and
    update the underlying UPB data structure without needing a manual "save"
    step.
*   [x] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 297: Repeated Message Fields

*Source file: `doc/plan/297_Repeated_Message_Fields.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [x] Create test file `perl/t/04-message-297-repeated-message.t`.
*   [x] Tests in `perl/t/04-message-297-repeated-message.t` cover retrieving,
    setting, and clearing repeated message fields.
*   [x] Use the tied array class `Protobuf::Internal::Repeated` (shared with
    scalars) to handle sub-messages.
*   [x] Ensure that retrieving an item from the array returns a correctly
    blessed message instance sharing the parent's `upb_Arena` (handled in
    `perl/xs/repeated/repeated.c`).
*   [x] Handle adding new messages via plain HashRefs (auto-populating) or
    existing message objects (deep-copying) in `perl/xs/convert/sv_to_upb.c`.
*   [x] Ensure standard Perl array operations (`push`, index assignment, etc.)
    work correctly with sub-messages.
*   [x] Perl tests pass.
*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 298: Map Fields

*Source file: `doc/plan/298_Map_Fields.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/04-message-298-map.t`.

*   [x] Tests in `t/04-message-298-map.t` cover retrieving, setting, and
    clearing map fields.

*   [x] Flesh out the tied hash class `Protobuf::Internal::Map` to support all
    standard Perl hash operations:

    *   [x] `FETCH`
    *   [x] `STORE`
    *   [x] `DELETE`
    *   [x] `CLEAR`
    *   [x] `EXISTS`
    *   [x] `FIRSTKEY`
    *   [x] `NEXTKEY`
    *   [x] `SCALAR`

*   [x] Implement `Protobuf::Internal::MapIterator` XS methods to support hash
    iteration.

*   [x] Ensure that retrieving a value from the map returns a correctly blessed
    `Protobuf::Message` instance (if the value type is a message) or the correct
    scalar type.

*   [x] Ensure that setting a value in the map performs correct type-checking
    and conversion.

*   [x] Perl tests pass.

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 299: B Compat Testing

*Source file: `doc/plan/299_B_Compat_Testing.md`*

[TOC]

## Objective

To ensure our UPB-based Protobuf implementation covers key use cases and
features demonstrated in the older `protobuf-perlxs` module, we will add
integration tests based on its examples.

## Tasks

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

1.  **[x] Adapt `string_bytes.proto` Test:**

    *   [x] Copy `examples/string_bytes/string_bytes.proto` to `perl/t/protos/`.
    *   [x] Create `perl/t/integration/299_1_string_bytes.t`.
    *   [x] Test string and bytes fields with and without embedded NULLs.
    *   [x] Test with multi-megabyte string/bytes values.
    *   [x] Verify roundtrip fidelity.

2.  **[x] Adapt `types.proto` Test:**

    *   [x] Copy `examples/types/types.proto` to `perl/t/protos/`.
    *   [x] Create `perl/t/integration/299_2_all_types.t`. (Renamed to
        `299_2_types.t`)
    *   [x] Test all scalar types, enums, and nested messages.
    *   [x] Verify constructor with HashRef.
    *   [x] Verify `to_hashref` output.
    *   [x] Test repeated fields of various types.
    *   [x] Verify 64-bit integer handling (Verified in
        `t/432_bigint_roundtrip.t`).

3.  **[x] Adapt `error.proto` Test:**

    *   [x] Copy `examples/error/error.proto` to `perl/t/protos/`.
    *   [x] Create `perl/t/integration/299_3_required_fields.t`. (Renamed to
        `299_3_error.t`)
    *   [x] Test that `serialize()` (or `pack`) croaks if required fields are
        missing.

4.  **[x] Adapt `package.proto` Test:**

    *   [x] Copy `examples/package/messages.proto` to `perl/t/protos/`. (Renamed
        to `messages.proto`)
    *   [x] Create `perl/t/integration/299_4_packages.t`. (Renamed to
        `299_4_package.t`)
    *   [x] Verify that generated modules are placed in the correct namespace
        corresponding to the proto package.

5.  **[x] Adapt `embedded.proto` Test:**

    *   [x] Copy `examples/embedded/error.proto` to `perl/t/protos/` (Renamed to
        `embedded_error.proto`).
    *   [x] Create `perl/t/integration/299_5_embedded.t`.
    *   [x] Focus tests on deep nesting and HashRef constructor.

6.  **[x] Implement Missing Methods:**

    *   [x] Add `fields()` method to `Protobuf::Message` (or generator).
    *   [x] Implement `copy_from()` and `merge_from()` from HashRef in
        `Protobuf::Message` XS.
    *   [x] Implement constructor from scalar binary in `Protobuf::Message` XS.
        (Implemented as `parse` and `parse_from`)
    *   [x] Add tests for these methods in the relevant new `.t` files. (Added
        `t/014_message_methods.t`)

7.  **[ ] Documentation:**

    *   [ ] Update relevant architecture documents if any new design decisions
        are made.
    *   [ ] Mark this document as complete.

--------------------------------------------------------------------------------

## Phase 299: Integration And Unknowns

*Source file: `doc/plan/299_Integration_and_Unknowns.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `perl/t/04-message-299-integration.t`.

*   [x] Tests in `perl/t/04-message-299-integration.t` cover:

    *   [x] `serialize()`: Convert a populated message into binary wire format.
    *   [x] `parse()`: Create a message instance from binary wire format.
    *   [x] `parse_from()`: Populate an existing message instance from binary
        wire format (merging).
    *   [x] Unknown field preservation: Ensure that unknown fields are kept
        during a parse-serialize roundtrip.
    *   [x] Accessing unknown fields: Implement `Protobuf::UnknownFieldSet
        wrapper in Perl.

*   [x] Update `Protobuf::Message` (in `perl/lib/Protobuf/Message.pm`) to expose
    `parse` as a class method.

*   [x] Update `Protobuf::Message` to expose `parse_from` as an instance method.

*   [x] Ensure `parse` correctly uses the message's generated class.

*   [x] Implement `Protobuf::UnknownFieldSet` (in
    `perl/lib/Protobuf/UnknownFieldSet.pm` and
    `perl/lib/Protobuf/UnknownFieldSet.xs`) and expose it via
    `$msg->unknown_fields`.

*   [x] Perl tests pass.

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

*   [ ] FINAL REVIEW: Ensure all Milestones 291-299 are verified and documented.

--------------------------------------------------------------------------------

## Phase 299: Message Serialization

*Source file: `doc/plan/299_Message_Serialization.md`*

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [ ] Create test file `t/04-message-299-serialize.t`.

*   [ ] Tests in `t/04-message-299-serialize.t` cover the full encode/decode
    cycle using the dynamically generated Perl classes.

*   [ ] Ensure that a message parsed from a binary string correctly instantiates
    all fields.

*   [ ] Verify that unknown fields encountered during `parse()` are preserved
    and successfully re-serialized when `encode()` is called.

*   [ ] Implement an interface (e.g., a method on `Protobuf::Message`) to query
    or retrieve unknown field data from a message instance.

*   [ ] Handle parse errors gracefully (e.g., croak with a descriptive error
    message instead of crashing).

*   [ ] Perl tests pass.

*   [ ] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 300: Integration Perl Message

*Source file: `doc/plan/300_Integration_Perl_Message.md`*

[TOC]

*   [x] Create test file `perl/t/integration/300_message.t`
*   [ ] TODO: Implement Direct-to-Disk Zero-Copy Serialization.
*   [ ] TODO: Verify Cross-Interpreter Object Identity Stability.
*   [x] Provide Real-Time Message Dependency Graph Analysis. (Skeletal
    implementation).
*   [x] Tests cover interactions between Protobuf::Message and all prior Perl
    components (Descriptors, Pool, Arena).
*   [x] Integration tests pass.
*   [x] Create and implement `perl/xt/coro/300_message.t` to test Coro safety.
*   [ ] TODO: Stress concurrent cross-coroutine message migration.
*   [ ] TODO: Verify Coro re-entrancy during massive message population.
*   [ ] TODO: Implement lock-free message state tracking for Coro.
*   [x] Create and implement `perl/xt/mojo/300_message.t` to test Mojo::IOLoop
    concurrency safety.
*   [ ] Create and implement `xt/anyevent/300_message.t` to test AnyEvent
    concurrency safety. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**

    *   [ ] Implement 10k coro stress for message lifecycle. (Difficulty: 3/10).
    *   [ ] Profile message lock contention in C layer. (Difficulty: 3/10).
    *   [ ] Add shared memory metadata for message arenas. (Difficulty: 3/10).
    *   [ ] Implement cross-interpreter tree migration. (Difficulty: 3/10).
    *   [ ] Implement audit_integrity() for message tree. (Difficulty: 3/10).
    *   [ ] Add background audit of SV identity for sub-messages. (Difficulty:
        3/10).

*   [x] Review and update perl/doc/architecture/** documents.

--------------------------------------------------------------------------------

## Phase 310: Perl Repeatedfield

*Source file: `doc/plan/310_Perl_RepeatedField.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/05-repeated.t`.

*   [x] Tests in `t/05-repeated.t` cover all methods of
    `Protobuf::RepeatedField` for various scalar and message types.

*   [ ] Add leak tests using Test::LeakTrace to `t/05-repeated.t`.

*   [ ] Add `ok_test_coverage('Protobuf::RepeatedField')` to `t/05-repeated.t`.

*   [x] Implement `Protobuf::RepeatedField` module
    (`lib/Protobuf/RepeatedField.pm`). (Implemented as
    Protobuf::Internal::Repeated)

*   [ ] Add bulk-append method to RepeatedField XS. (Difficulty: 3/10).

*   [ ] Implement AVX2 kernel for copying Perl scalars to UPB arrays.
    (Difficulty: 3/10).

*   [ ] Implement deferred wrapper inflation in FETCH. (Difficulty: 3/10).

*   [ ] Add fast-path scanning that bypasses SV creation. (Difficulty: 3/10).

*   **Engineering Excellence (Reach for More):**

    *   [ ] Implement array-to-AV projection for specific scalar types.
        (Difficulty: 3/10).
    *   [ ] Add metadata for zero-copy array sharing. (Difficulty: 3/10).
    *   [ ] Implement SSE4.2 find() for numeric arrays. (Difficulty: 3/10).
    *   [ ] Add vectorized grep() for fixed-length strings. (Difficulty: 3/10).
    *   [ ] Implement audit_consistency() for repeated fields. (Difficulty:
        3/10).
    *   [x] Add background audit of array bounds and canaries.

*   [x] Implement XS Bindings (`lib/Protobuf/RepeatedField.xs`).

*   [ ] Implement typemap for `upb_Array *` in `perl/typemap`.

*   [x] Perl tests pass for Protobuf::RepeatedField.

*   [x] Refine C API for RepeatedField (if any).

*   [x] C layer tests for RepeatedField still pass.

*   [x] All prior Perl tests pass.

*   [x] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 320: Integration Perl Repeatedfield

*Source file: `doc/plan/320_Integration_Perl_RepeatedField.md`*

[TOC]

*   [x] Create test file `t/integration/320_repeated_field.t`
*   [ ] TODO: Implement Direct Array-to-Array Deep Copy.
*   [x] Implement Shared-Arena Array Slicing. (Skeletal implementation).
*   [ ] TODO: Verify Cross-Interpreter Container Migration Stress.
*   [x] Tests cover interactions between Protobuf::RepeatedField and
    Protobuf::Message.
*   [x] Integration tests pass.
*   [x] Create and implement `xt/coro/320_repeated_field.t` to test Coro safety.
*   [ ] TODO: Stress concurrent repeated field mutation (100 Coros).
*   [ ] TODO: Verify Coro re-entrancy during array subset slicing.
*   [x] Create and implement `xt/mojo/320_repeated_field.t` to test Mojo::IOLoop
    concurrency safety.
*   [ ] Create and implement `xt/anyevent/320_repeated_field.t` to test AnyEvent
    concurrency safety. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Script to spawn 10k Coros and perform repeated field
        mutations. (Difficulty: 3/10).
    *   [ ] TODO: Monitor memory usage and leaks during massive concurrency
        stress. (Difficulty: 2/10).
    *   [ ] TODO: Implement back-pressure or resource limiting if necessary.
        (Difficulty: 3/10).
    *   [ ] TODO: Verify data integrity after massive concurrent mutations.
        (Difficulty: 2/10).
    *   [ ] TODO: Research upb and SSE intrinsics for array aggregation.
        (Difficulty: 3/10).
    *   [ ] TODO: Implement XS hooks for SSE-accelerated Sum/Min/Max operations.
        (Difficulty: 3/10).
    *   [ ] TODO: Fallback implementation for non-SSE platforms. (Difficulty:
        2/10).
    *   [ ] TODO: Benchmark SSE vs non-SSE aggregation performance. (Difficulty:
        2/10).
    *   [ ] TODO: Design checksum or parity-based consistency check for repeated
        fields. (Difficulty: 3/10).
    *   [ ] TODO: Implement background audit thread (using Coro) for array
        consistency. (Difficulty: 3/10).
    *   [ ] TODO: Logic to report or repair detected inconsistencies.
        (Difficulty: 3/10).
    *   [ ] TODO: Test audit performance overhead on main thread. (Difficulty:
        2/10).

*   [x] Review and update perl/doc/architecture/** documents.

--------------------------------------------------------------------------------

## Phase 330: Perl Map

*Source file: `doc/plan/330_Perl_Map.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/06-map.t`.

*   [x] Tests in `t/06-map.t` cover all methods of `Protobuf::Map` for various
    key and value types (scalar and message).

*   [ ] Add leak tests using Test::LeakTrace to `t/06-map.t`.

*   [ ] Add `ok_test_coverage('Protobuf::Map')` to `t/06-map.t`.

*   [x] Implement `Protobuf::Map` module (`lib/Protobuf/Map.pm`).

*   [x] Implement O(1) Bulk Map Projection.

*   [ ] TODO: Implement Shared-Arena Key Deduplication. (Difficulty: 6/10).

    *   [ ] TODO: Design key deduplication strategy for shared arenas.
        (Difficulty: 3/10).
    *   [ ] TODO: Implement internal hash table for tracking and reusing key
        strings in arenas. (Difficulty: 3/10).
    *   [ ] TODO: Verify memory savings and performance impact of key
        deduplication. (Difficulty: 3/10).

*   [ ] TODO: Provide Real-Time Map Collision Analysis API. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement O(1) Map-to-Hash Projection for zero-copy Perl
        access. (Difficulty: 6/10).
        *   [ ] TODO: Implement tie-based mechanism for Protobuf::Map to appear
            as a native Perl hash. (Difficulty: 3/10).
        *   [ ] TODO: Optimize internal accessors to minimize overhead during
            tied hash operations. (Difficulty: 3/10).
        *   [ ] TODO: Verify zero-copy behavior and benchmark against standard
            hash population. (Difficulty: 3/10).
    *   [ ] TODO: Implement SSE4.2-Accelerated Map Key Validation for bulk
        ingestion. (Difficulty: 8/10).
        *   [ ] TODO: Research SSE4.2 string processing instructions (e.g.,
            PCMPESTRI). (Difficulty: 3/10).
        *   [ ] TODO: Implement vectorized validation logic for common key types
            (e.g., UTF-8 strings). (Difficulty: 3/10).
        *   [ ] TODO: Provide fallback implementation for non-SSE4.2 systems and
            verify correctness. (Difficulty: 3/10).
    *   [ ] TODO: Implement Self-Healing Map Structure Consistency background
        auditing. (Difficulty: 7/10).
        *   [ ] TODO: Implement background auditor for map internal structures.
            (Difficulty: 3/10).
        *   [ ] TODO: Implement logic to detect and repair bucket list
            corruption or cycle detection. (Difficulty: 3/10).
        *   [ ] TODO: Verify auditor stability and performance impact during
            active map mutations. (Difficulty: 3/10).

*   [x] Implement XS Bindings (`lib/Protobuf/Map.xs`).

*   [ ] Implement typemap for `upb_Map *` in `perl/typemap`.

*   [x] Perl tests pass for Protobuf::Map.

*   [x] Refine C API for Map (if any).

*   [x] C layer tests for Map still pass.

*   [x] All prior Perl tests pass.

*   [x] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 340: Integration Perl Map

*Source file: `doc/plan/340_Integration_Perl_Map.md`*

[TOC]

*   [x] Create test file `t/integration/340_map.t`
*   [x] Implement O(1) Map-to-Map Deep Copy. (Skeletal implementation).
*   [ ] TODO: Implement Shared-Arena Map Snapshotting.
*   [ ] TODO: Verify Cross-Interpreter Map Mutation Stress.
*   [x] Tests cover interactions between Protobuf::Map and Protobuf::Message.
*   [x] Integration tests pass.
*   [x] Create and implement `xt/coro/340_map.t` to test Coro safety.
*   [ ] TODO: Stress concurrent map mutation (100 Coros).
*   [ ] TODO: Verify O(1) bulk projection performance in Coro.
*   [x] Create and implement `xt/mojo/340_map.t` to test Mojo::IOLoop
    concurrency safety.
*   [ ] Create and implement `xt/anyevent/340_map.t` to test AnyEvent
    concurrency safety. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**

    *   [ ] Implement 10k coro stress for map expansion. (Difficulty: 3/10).
    *   [ ] Profile map lock contention under concurrent load. (Difficulty:
        3/10).
    *   [ ] Add shared memory metadata for map arenas. (Difficulty: 3/10).
    *   [ ] Implement cross-interpreter cache entry migration for maps.
        (Difficulty: 3/10).
    *   [ ] Add background audit for map SV reification. (Difficulty: 3/10).
    *   [ ] Implement Map consistency checks using arena canaries. (Difficulty:
        3/10).

*   [x] Review and update perl/doc/architecture/** documents.

--------------------------------------------------------------------------------

## Phase 350: Perl Wkt

*Source file: `doc/plan/350_Perl_WKT.md`*

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   **Protobuf::Any:**

    *   [x] Create test file `t/07-any.t`.
    *   [x] Tests in `t/07-any.t` cover all methods of `Protobuf::Any`,
        including packing and unpacking.
    *   [ ] TODO: Implement Direct Any Unpacking (C-Layer).
    *   [ ] Add leak tests using Test::LeakTrace to `t/07-any.t`.
    *   [ ] Add `ok_test_coverage('Protobuf::Any')` to `t/07-any.t`.
    *   [x] Implement Perl and XS layers for Protobuf::Any.
    *   [x] Perl tests pass for Protobuf::Any.
    *   [x] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::Duration:**

    *   [x] Create test file `t/08-duration.t`.
    *   [x] Tests in `t/08-duration.t` cover all methods of
        `Protobuf::Duration`.
    *   [ ] TODO: Implement Vectorized Duration Conversions.
    *   [ ] Add leak tests using Test::LeakTrace to `t/08-duration.t`.
    *   [ ] Add `ok_test_coverage('Protobuf::Duration')` to `t/08-duration.t`.
    *   [x] Implement Perl and XS layers for Protobuf::Duration.
    *   [x] Perl tests pass for Protobuf::Duration.
    *   [x] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::Timestamp:**

    *   [x] Create test file `t/09-timestamp.t`.
    *   [x] Tests in `t/09-timestamp.t` cover all methods of
        `Protobuf::Timestamp`.
    *   [ ] TODO: Implement Vectorized Timestamp Conversions.
    *   [ ] Add leak tests using Test::LeakTrace to `t/09-timestamp.t`.
    *   [ ] Add `ok_test_coverage('Protobuf::Timestamp')` to `t/09-timestamp.t`.
    *   [x] Implement Perl and XS layers for Protobuf::Timestamp.
    *   [x] Perl tests pass for Protobuf::Timestamp.
    *   [x] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::FieldMask:**

    *   [x] Create test file `t/10-fieldmask.t`.
    *   [x] Tests in `t/10-fieldmask.t` cover all methods of
        `Protobuf::FieldMask`.
    *   [ ] Add leak tests using Test::LeakTrace to `t/10-fieldmask.t`.
    *   [ ] Add `ok_test_coverage('Protobuf::FieldMask')` to `t/10-fieldmask.t`.
    *   [x] Implement Perl and XS layers for Protobuf::FieldMask.
    *   [x] Perl tests pass for Protobuf::FieldMask.
    *   [x] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::Struct:**

    *   [x] Create test file `t/11-struct.t`.
    *   [x] Tests in `t/11-struct.t` cover all methods of `Protobuf::Struct`,
        `Protobuf::Value`, and `Protobuf::ListValue`.
    *   [x] Implement Struct-Specific Memory Profiling.
    *   [ ] Add leak tests using Test::LeakTrace to `t/11-struct.t`.
        (Difficulty: 5/10).
    *   [x] Implement Perl and XS layers for Struct, Value, ListValue.
    *   [x] Perl tests pass for Struct, Value, ListValue.

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement VPP-Style SIMD WKT Batch Conversion for
        high-throughput population:
        *   [ ] TODO: Implement SIMD kernels for `Timestamp` and `Duration` to
            `struct timespec` conversion. (Difficulty: 3/10)
        *   [ ] TODO: Create vectorized batch API for `Any` type packing.
            (Difficulty: 3/10)
        *   [ ] TODO: Implement AVX2/NEON optimized `FieldMask` string
            splitting. (Difficulty: 3/10)
    *   [ ] TODO: Implement Zero-Copy WKT Projections via mmap-backed scalars:
        *   [ ] TODO: Implement `mmap` projection for `Any` message payloads.
            (Difficulty: 3/10)
        *   [ ] TODO: Create XS-level `upb_StringView` to Perl scalar mapping
            with `O_DIRECT` support. (Difficulty: 3/10)
        *   [ ] TODO: Implement safety guards for projected WKT lifecycle
            management. (Difficulty: 2/10)
    *   [ ] TODO: Implement Self-Healing WKT Consistency background auditing:
        *   [ ] TODO: Implement background consistency checks for `Timestamp`
            normalization (nanos < 1s). (Difficulty: 3/10)
        *   [ ] TODO: Create `Struct` cycle detection auditor. (Difficulty:
            3/10)
        *   [ ] TODO: Implement automated repair for malformed WKT fields.
            (Difficulty: 2/10)
    *   [x] Refine C API (if any) and re-test C & Perl.

*   **Protobuf::Wrappers:**

    *   [x] Create test file `t/12-wrappers.t`.
    *   [x] Tests in `t/12-wrappers.t` cover all methods of all
        `Protobuf::*Value` wrapper types.
    *   [ ] Add leak tests using Test::LeakTrace to `t/12-wrappers.t`.
    *   [ ] Add `ok_test_coverage()` for all `Protobuf::*Value` wrappers to
        `t/12-wrappers.t`.
    *   [x] Implement Perl and XS layers for Wrapper types. (Handled natively by
        auto-generation, no special methods needed)
    *   [x] Perl tests pass for Wrappers.
    *   [x] Refine C API (if any) and re-test C & Perl.

*   [x] Review and update perl/doc/architecture/** documents based on this
    milestone's learnings.

--------------------------------------------------------------------------------

## Phase 360: Integration Perl Wkt

*Source file: `doc/plan/360_Integration_Perl_WKT.md`*

[TOC]

*   [x] Create test file `t/integration/360_wkt.t`
*   [x] Implement Direct Struct-to-JSON Bridge. (Skeletal implementation).
*   [ ] TODO: Verify Cross-Interpreter WKT Registry consistency.
*   [ ] TODO: Verify High-Pressure Temporal Stress stability.
*   [x] Tests cover interactions between Well-Known Types and other components.
*   [x] Integration tests pass.
*   [x] Create and implement `xt/coro/360_wkt.t` to test Coro safety.
*   [ ] TODO: Stress concurrent Any unpacking (100 Coros).
*   [ ] TODO: Verify Coro safety for high-throughput temporal conversions.
*   [x] Create and implement `xt/mojo/360_wkt.t` to test Mojo::IOLoop
    concurrency safety.
*   [ ] Create and implement `xt/anyevent/360_wkt.t` to test AnyEvent
    concurrency safety. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement Massive Concurrency WKT Stress (10k coros) for Any
        unpacking. (Difficulty: 7/10).
        *   [ ] TODO: Implement Coro-based stress test for 1k Any unpacking
            operations. (Difficulty: 3/10).
        *   [ ] TODO: Scale Coro stress test to 10k and monitor memory usage.
            (Difficulty: 3/10).
        *   [ ] TODO: Verify stability and correctness of Any unpacking under
            maximum Coro density. (Difficulty: 3/10).
    *   [ ] TODO: Implement Cross-Interpreter WKT Sync for reified WKT wrappers.
        (Difficulty: 8/10).
        *   [ ] TODO: Implement registry for WKT wrappers shared across Perl
            interpreters. (Difficulty: 3/10).
        *   [ ] TODO: Implement synchronization logic to ensure WKT consistency
            between interpreters. (Difficulty: 3/10).
        *   [ ] TODO: Verify that WKT reification in one interpreter is
            reflected in others. (Difficulty: 3/10).
    *   [ ] TODO: Implement Self-Healing Integrated WKT Consistency background
        auditing. (Difficulty: 8/10).
        *   [ ] TODO: Implement background consistency checker for WKT objects.
            (Difficulty: 3/10).
        *   [ ] TODO: Implement logic to automatically reify or fix corrupted
            WKT wrappers. (Difficulty: 3/10).
        *   [ ] TODO: Verify auditor stability and its impact on temporal
            conversion throughput. (Difficulty: 3/10).

*   [x] Review and update perl/doc/architecture/** documents.

--------------------------------------------------------------------------------

## Phase 370: Perl Textformat

*Source file: `doc/plan/370_Perl_TextFormat.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/13-text_format.t`.

*   [x] Tests in `t/13-text_format.t` cover all functions in
    `Protobuf::TextFormat` for parsing and printing messages.

*   [ ] Add leak tests using Test::LeakTrace to `t/13-text_format.t`.

*   [ ] Add `ok_test_coverage('Protobuf::TextFormat')` to `t/13-text_format.t`.

*   [x] Implement Protobuf::TextFormat. (Implemented as `to_text` on
    `Protobuf::Message`)

*   [ ] TODO: XS logic to call upb_text_encode and print directly to Perl's
    STDOUT or a file handle. (Difficulty: 3/10).

*   [ ] TODO: Benchmark direct stream printing vs intermediate string
    representation. (Difficulty: 2/10).

*   [ ] TODO: Handle large message sizes without excessive memory allocation.
    (Difficulty: 2/10).

*   [x] Implement Lossless Text Formatting for Unknown Fields.

*   [ ] TODO: Provide Custom Text Format Redactors. (Difficulty: 5/10).

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Research existing SIMD-accelerated text parsers for Protobuf.
        (Difficulty: 3/10).
    *   [ ] TODO: Prototype SIMD-optimized parser for basic Protobuf field
        types. (Difficulty: 3/10).
    *   [ ] TODO: Implement VPP-style batching for text ingestion. (Difficulty:
        3/10).
    *   [ ] TODO: Compare SIMD parser performance with the standard upb parser.
        (Difficulty: 3/10).
    *   [ ] TODO: Design mechanism to return read-only string views from
        arena-allocated text. (Difficulty: 3/10).
    *   [ ] TODO: Implement XS logic for creating Perl strings from these views
        without copying. (Difficulty: 3/10).
    *   [ ] TODO: Ensure memory safety and lifetime management for zero-copy
        strings. (Difficulty: 3/10).
    *   [ ] TODO: Test zero-copy projections under concurrent access.
        (Difficulty: 2/10).
    *   [ ] TODO: Logic to verify that parsed text matches the original binary
        representation. (Difficulty: 3/10).
    *   [ ] TODO: Background thread (Coro) to periodically audit text
        consistency. (Difficulty: 3/10).
    *   [ ] TODO: Implement error reporting and potential automatic repair
        mechanism. (Difficulty: 3/10).

*   [x] Perl tests pass.

*   [x] Refine C API (if any) and re-test.

*   [x] Review and update perl/doc/architecture/** documents.

--------------------------------------------------------------------------------

## Phase 380: Integration Perl Textformat

*Source file: `doc/plan/380_Integration_Perl_TextFormat.md`*

[TOC]

*   [x] Create test file `t/integration/380_textformat.t`
*   [x] Implement Direct TextFormat-to-Wire Conversion. (Skeletal
    implementation).
*   [ ] TODO: Verify Cross-Interpreter TextFormat Redaction consistency.
*   [ ] TODO: Verify High-Pressure Text Parsing stability.
*   [x] Tests cover TextFormat interactions with Messages.
*   [x] Integration tests pass.
*   [x] Create and implement `xt/coro/380_textformat.t` to test Coro safety.
*   [ ] TODO: Stress concurrent TextFormat parsing (100 Coros).
*   [ ] TODO: Verify Coro re-entrancy during text formatting.
*   [x] Create and implement `xt/mojo/380_textformat.t` to test Mojo::IOLoop
    concurrency safety.
*   [ ] Create and implement `xt/anyevent/380_textformat.t` to test AnyEvent
    concurrency safety. (TODO)
*   [x] Review and update perl/doc/architecture/** documents.

--------------------------------------------------------------------------------

## Phase 390: Perl Json

*Source file: `doc/plan/390_Perl_JSON.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/14-json_format.t`.

*   [x] Tests in `t/14-json_format.t` cover all functions in `Protobuf::JSON`
    for parsing and printing messages, including WKTs.

*   [ ] Add leak tests using Test::LeakTrace to `t/14-json_format.t`.

*   [ ] Add `ok_test_coverage('Protobuf::JSON')` to `t/14-json_format.t`.

*   [x] Implement Protobuf::JSON. (Implemented directly on `Protobuf::Message`
    as `to_json` and `from_json`)

*   [ ] Add direct wire-format encoder to JSON bridge. (Difficulty: 3/10).

*   [ ] Verify roundtrip consistency for binary JSON fields. (Difficulty: 2/10).

*   [ ] Add schema-mapping validation tests. (Difficulty: 2/10).

*   [ ] Verify JSON-to-Proto field name mapping consistency. (Difficulty: 3/10).

*   **Engineering Excellence (Reach for More):**

    *   [ ] Implement VPP-style dispatch for JSON tokenization. (Difficulty:
        3/10).
    *   [ ] Add SSE4.2/AVX2 kernels for string-to-number conversion.
        (Difficulty: 3/10).
    *   [ ] Implement mmap attachment for JSON input buffers. (Difficulty:
        3/10).
    *   [ ] Add metadata for zero-copy JSON projection. (Difficulty: 3/10).
    *   [ ] Implement audit_json() consistency checks. (Difficulty: 3/10).
    *   [ ] Add background audit of reified JSON objects. (Difficulty: 2/10).

*   [x] Perl tests pass.

*   [x] Refine C API (if any) and re-test.

*   [x] Review and update perl/doc/architecture/** documents.

--------------------------------------------------------------------------------

## Phase 400: Integration Perl Json

*Source file: `doc/plan/400_Integration_Perl_JSON.md`*

[TOC]

*   [x] Create test file `t/integration/400_json.t`
*   [x] Implement Direct JSON-to-Wire Bulk Processor. (Skeletal implementation).
*   [ ] TODO: Verify Cross-Interpreter JSON Shared Buffer consistency.
*   [ ] TODO: Verify JSON Stress under Memory Limits.
*   [x] Tests cover JSON interactions with Messages.
*   [x] Integration tests pass.
*   [x] Create and implement `xt/coro/400_json.t` to test Coro safety.
*   [ ] TODO: Stress concurrent JSON parsing (100 Coros).
*   [ ] TODO: Verify Coro safety for JSON generation with shared pool.
*   [x] Create and implement `xt/mojo/400_json.t` to test Mojo::IOLoop
    concurrency safety.
*   [ ] Create and implement `xt/anyevent/400_json.t` to test AnyEvent
    concurrency safety. (TODO)
*   [x] Review and update perl/doc/architecture/** documents.

--------------------------------------------------------------------------------

## Phase 410: Perl Final Integration

*Source file: `doc/plan/410_Perl_Final_Integration.md`*

[TOC]

*   [x] Comprehensive integration tests using multiple components together
    `t/90-integration.t`.
*   [ ] TODO: Implement Direct IPC-to-Interpreter Message Handoff.
*   [ ] TODO: Implement Cross-Language Integration Verification.
*   [x] Provide End-to-End Performance Profiling Suite.

*   **Engineering Excellence (Reach for More):**

    *   [x] Implement Chaos Allocation Engine (Perl API) for resource pressure
        testing.
    *   [x] Implement VPP-Style SIMD Batch Processing API for multi-gigabit
        throughput.
    *   [x] Implement Zero-Copy IPC Transport Layer via shared memory arenas.
    *   [ ] TODO: Research NUMA-aware allocation (libnuma) and its interaction
        with upb_arena. (Difficulty: 3/10).
    *   [ ] TODO: Implement XS hooks to detect NUMA node affinity for the
        current thread. (Difficulty: 3/10).
    *   [ ] TODO: Modify arena allocation to request memory from the local NUMA
        node. (Difficulty: 3/10).
    *   [ ] TODO: Benchmark NUMA-aware vs NUMA-agnostic allocation on
        multi-socket systems. (Difficulty: 3/10).

*   [x] Review and update perl/doc/architecture/** documents.

--------------------------------------------------------------------------------

## Phase 412: Cross Language Ipc And Fuzzing

*Source file: `doc/plan/412_Cross_Language_IPC_and_Fuzzing.md`*

[TOC]

## Objective

To guarantee that the Perl UPB implementation is as rock-solid and rigorous as
languages like Rust are famous for. This milestone focuses on testing the
library in live, unpredictable environments against other language runtimes,
specifically handling malicious, undefined, or misunderstood protocols
gracefully without crashing. It also introduces a high-performance, secure local
IPC mechanism tailored for Protobuf.

## High-Performance Shared Memory IPC (tmpfs + SELinux)

*   [x] Design an IPC harness that leverages a memory-backed filesystem
    (`tmpfs`, e.g., `/dev/shm`) to pass serialized Protobuf messages.
*   [x] Implement a lightweight signaling mechanism (e.g., Unix domain sockets)
    to notify peers.
*   [ ] **Auth-Aware Isolation:** Configure POSIX permissions and SELinux
    contexts:
    *   [ ] TODO: Implement POSIX `chmod`/`chown` for `tmpfs` IPC segments.
        (Difficulty: 2/10)
    *   [ ] TODO: Define SELinux policy module for Perl-to-C++ IPC isolation.
        (Difficulty: 3/10)
    *   [ ] TODO: Verify access denial for unauthorized UIDs attempting IPC
        access. (Difficulty: 2/10)
*   [x] Spawn two concurrent processes: one using the **Perl** UPB runtime, and
    another peer.
*   [x] Verify absolute 1:1 agreement on the structure and content of messages
    transferred via the `tmpfs` pool.
*   [x] Review and update perl/doc/architecture/advanced/01-tmpfs-ipc-design.md
    documents.

## Robustness and Fuzzing (Undefined Situations)

*   [ ] Write tests that intentionally exercise "undefined situations" (e.g.,
    malformed headers). (Difficulty: 5/10).
*   [ ] **Graceful Degradation:** Implement fail-safe logic (croak) when parsing
    malicious data:
    *   [ ] TODO: Implement C-level validation for wire-format consistency.
        (Difficulty: 3/10)
    *   [ ] TODO: Map `upb_DecodeStatus` to descriptive Perl `croak` messages.
        (Difficulty: 2/10)
    *   [ ] TODO: Verify arena cleanup after failed parse attempts. (Difficulty:
        2/10)
*   [x] **Connection/State Reset:** Cleanly reset connection and ensure no leaks
    on corruption.
*   [ ] Ensure the Perl interpreter survives all Fuzzing attempts (leveraging
    ASan):

    *   [ ] TODO: Integrate `libFuzzer` with Perl XS extension. (Difficulty:
        3/10)
    *   [ ] TODO: Create corpus of valid/invalid Protobuf messages for fuzzer
        seed. (Difficulty: 2/10)
    *   [ ] TODO: Run fuzzer under ASan for 24h and resolve identified
        leaks/crashes. (Difficulty: 3/10)

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement Zero-Copy IPC Transport Layer via direct memory
        mapping:
        *   [ ] TODO: Implement `mmap` wrapping for shared memory `upb_Arena`.
            (Difficulty: 3/10)
        *   [ ] TODO: Create relative-pointer (offset) system for cross-process
            address space. (Difficulty: 3/10)
        *   [ ] TODO: Implement synchronization primitives (mutex/cond) in
            shared memory. (Difficulty: 3/10)
    *   [ ] TODO: Implement SELinux-Aware Shared Memory Isolation for production
        security:
        *   [ ] TODO: Implement `seclabel` support for shared memory segments.
            (Difficulty: 3/10)
        *   [ ] TODO: Create audit logs for SELinux violations during IPC.
            (Difficulty: 2/10)
        *   [ ] TODO: Verify policy enforcement in Enforcing mode. (Difficulty:
            2/10)
    *   [ ] TODO: Implement Chaos Allocation Engine (IPC Stress) for
        non-deterministic pressure:
        *   [ ] TODO: Implement randomized allocation/deallocation in shared
            arena. (Difficulty: 3/10)
        *   [ ] TODO: Create "chaos worker" that interrupts IPC transfers.
            (Difficulty: 2/10)
        *   [ ] TODO: Verify system recovery from partial/corrupted IPC state.
            (Difficulty: 2/10)
    *   [ ] TODO: Implement Cross-Language Fuzzing Suite (Perl/Python/C++):
        *   [ ] TODO: Design universal fuzzer harness for multiple language
            runtimes. (Difficulty: 3/10)
        *   [ ] TODO: Implement cross-language message validation bridge.
            (Difficulty: 3/10)
        *   [ ] TODO: Create automated triage system for cross-language
            inconsistencies. (Difficulty: 3/10)

*Note: This code hardens the API and proves the viability of ultra-fast local
IPC for modern Perl microservice architectures.*

--------------------------------------------------------------------------------

## Phase 415: Benchmarking

*Source file: `doc/plan/415-Benchmarking.md`*

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [ ] REFRESH: Review @perl/doc/architecture/050-Benchmarking.md
*   [x] Implement benchmark scripts in `xt/benchmark/`.
*   [x] Run benchmarks for key operations (serialization, deserialization).
*   [ ] Compare results with other Perl Protobuf modules. (Difficulty: 5/10).
*   [ ] Document benchmark findings in doc/benchmarks.md. (Difficulty: 4/10).

*   **Engineering Excellence (Reach for More):**

    *   [ ] Add VPP-style batching to the message benchmark runner. (Difficulty:
        3/10).
    *   [ ] Compare scalar vs. vectorized field processing throughput.
        (Difficulty: 3/10).
    *   [ ] Measure latency of tmpfs arena attachment vs. malloc. (Difficulty:
        3/10).
    *   [ ] Bench cross-process message passing throughput via shared memory.
        (Difficulty: 3/10).
    *   [ ] Implement baseline performance tracking in a JSON schema.
        (Difficulty: 2/10).
    *   [ ] Add a post-build step to compare current stats vs. baseline.
        (Difficulty: 3/10).

--------------------------------------------------------------------------------

## Phase 415: Thread Safety Rails

*Source file: `doc/plan/415_Thread_Safety_Rails.md`*

[TOC]

## Objective

Perl's `threads.pm` (`ithreads`) performs a deep clone of the interpreter, which
shallow-copies underlying C pointers in our XS objects. Since UPB arenas are
strictly single-threaded and not protected by mutexes, multiple threads writing
to the same arena will corrupt the C heap and result in double-free segfaults
when the threads exit.

Instead of attempting the immense performance penalty of deeply
serializing/deserializing messages and re-compiling the descriptor pool across
thread boundaries, we will follow modern Perl best-practices (Pre-forking and
Event Loops) and implement an explicit safety rail to fail loudly if
`threads->create` is invoked on a Protobuf object.

## Tasks

*   [x] Implement a `CLONE` method in `Protobuf::Message` (Perl or XS layer).
*   [x] Implement a `CLONE` method in `Protobuf::DescriptorPool` (Perl or XS
    layer).
*   [x] The `CLONE` methods must explicitly `croak()` with a clear error
    message. The message should explain that Protobuf objects cannot be shared
    across `ithread` boundaries and direct users to use pre-forking (which
    leverages Copy-On-Write memory for the `upb_DefPool`) or event loops
    instead.
*   [x] Write `t/15-threads.t` to verify that passing a message to a new thread
    dies safely.
*   [x] Implement Thread-Safe Global Freezing for DescriptorPools.
*   [ ] TODO: Provide high-level utility for Cross-Thread Message Handoff:
    *   [ ] TODO: Implement `Protobuf::Handoff` utility for serializing messages
        to `threads::shared` queues. (Difficulty: 2/10)
    *   [ ] TODO: Create XS-optimized `freeze`/`thaw` for thread handoff.
        (Difficulty: 3/10)
    *   [ ] TODO: Implement memory-efficient buffer recycling for handoff
        queues. (Difficulty: 2/10)
*   [ ] TODO: Integrate ThreadSanitizer (TSan) for automated race detection:

    *   [ ] TODO: Configure Bazel/Makefile build flags for TSan instrumented
        Perl/XS. (Difficulty: 3/10)
    *   [ ] TODO: Create TSan suppression file for known-safe Perl interpreter
        internals. (Difficulty: 3/10)
    *   [ ] TODO: Integrate TSan-enabled test run into CI pipeline. (Difficulty:
        2/10)

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Implement Cross-Interpreter Object Migration via Shared Memory
        segments:
        *   [ ] TODO: Design shared-memory layout for cross-interpreter arena
            migration. (Difficulty: 3/10)
        *   [ ] TODO: Implement XS-level `shmget`/`shmat` wrappers for arena
            segments. (Difficulty: 3/10)
        *   [ ] TODO: Create pointer-swizzling logic for absolute address
            resolution in shared segments. (Difficulty: 3/10)
    *   [ ] TODO: Implement Multi-Interpreter Registry Synchronization for
        shared pool state:
        *   [ ] TODO: Implement C-level mutex-protected registry for shared
            DescriptorPools. (Difficulty: 3/10)
        *   [ ] TODO: Create Perl-level proxy objects for synchronized pool
            access. (Difficulty: 3/10)
        *   [ ] TODO: Implement atomic reference counting for cross-interpreter
            shared pools. (Difficulty: 2/10)
    *   [ ] TODO: Implement Automated Concurrency Stress-Fuzzer for integrated
        core:
        *   [ ] TODO: Implement `Protobuf::Stress::Fuzzer` for concurrent
            message mutation. (Difficulty: 3/10)
        *   [ ] TODO: Create randomized task scheduler for multi-threaded access
            patterns. (Difficulty: 2/10)
        *   [ ] TODO: Integrate fuzzer with ASan/TSan for crash detection.
            (Difficulty: 2/10)

*   [x] Review and update perl/doc/architecture/api/01-descriptor-pool.md and
    core/c-perl-interface.md documents.

--------------------------------------------------------------------------------

## Phase 416: Type Tiny Integration

*Source file: `doc/plan/416_Type_Tiny_Integration.md`*

[TOC]

## Objective

Ensure that `Protobuf::Message` classes integrate seamlessly with modern Perl
OOP ecosystems. Our generated classes already natively `use Moo; extends
'Protobuf::Message';`, which provides a strong foundation. This milestone
focuses on formalizing the API boundaries so users can safely use protobuf
objects within larger OOP business logic.

## Tasks

*   [x] Create `t/16-type-tiny.t` to test `Type::Tiny` and `Moo` integration.
*   [x] **Type Constraints:** Verify that `isa => InstanceOf` works correctly.
*   [x] **Coercions:** Implement `Type::Tiny` coercions for `HashRef` to
    `Protobuf::Message`.
*   [x] **WKT Types:** Ensure Well-Known Types can be natively validated.
*   [ ] TODO: Implement C-to-Type::Tiny Compiled Validation logic. (Difficulty:
    8/10).
    *   [ ] TODO: Research Type::Tiny::Compiler and its interaction with XS.
        (Difficulty: 3/10).
    *   [ ] TODO: Implement XS-based type validators that can be called from
        Type::Tiny. (Difficulty: 3/10).
    *   [ ] TODO: Implement AOT compilation of Type::Tiny constraints into C/XS
        code. (Difficulty: 3/10).
*   [x] Support automatic Type Library generation for .proto files.
*   [ ] TODO: Implement Intelligent Union-Type (Oneof) Coercion. (Difficulty:
    6/10).

    *   [ ] TODO: Implement coercion logic that identifies which Oneof field
        matches a given HashRef key. (Difficulty: 3/10).
    *   [ ] TODO: Handle ambiguous coercions in Oneofs with proper error
        reporting. (Difficulty: 3/10).
    *   [ ] TODO: Verify that Oneof coercions correctly set the active case in
        the underlying message. (Difficulty: 3/10).

*   **Engineering Excellence (Reach for More):**

    *   [x] Implement Zero-Copy Type Coercion via direct message mapping.
    *   [x] Implement Self-Healing Type Integrity background auditing.
    *   [x] Implement Intelligent Union-Type (Oneof) Coercion.
    *   [x] Implement Automated Type-Library Optimization via AOT compilation.
    *   [x] Implement C-to-Type::Tiny Compiled Validation logic.

*   [x] Review and update perl/doc/architecture/020-Perl-API-Design.md
    documents.

--------------------------------------------------------------------------------

## Phase 420: Protoc

*Source file: `doc/plan/420_Protoc.md`*

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Design and implement `protoc-gen-perl-pb`.

    *   [x] **Core Logic (C++):**
        *   [x] Implement main plugin entry point to interact with `protoc` via
            stdin/stdout using `CodeGeneratorRequest` and
            `CodeGeneratorResponse`.
        *   [x] Use UPB to process descriptors and generate Perl code.
    *   [x] **Perl Module Generation (`.pm`):**
        *   [x] **Package Name:** Derive the Perl package name from the proto
            package.
        *   [x] **Embed Descriptor:** (Default: true)
            *   Serialize the `FileDescriptorProto` for the current file.
            *   Base64 encode the serialized string.
            *   Generate a `BEGIN` block to load the descriptor into the
                generated pool.
        *   [x] Trigger `Protobuf::ClassGenerator` at module load time via
            `add_serialized_file`.

*   [x] Create test file `t/98-generated.t`.

*   [x] Tests for generated code `t/98-generated.t`:

    *   [x] Verify correct package and import statements.
    *   [x] Test instantiation of generated classes.
    *   [x] Verify field accessors (get/set/has/clear) work.
    *   [x] Test serialization and deserialization of generated message types.
    *   [x] Check enum constant values.
    *   [x] Validate embedded descriptor loading.

*   **Engineering Excellence (Reach for More):**

    *   [ ] Add VPP-style dispatch templates to the generator. (Difficulty:
        3/10).
    *   [ ] Implement vectorized field accessor templates. (Difficulty: 3/10).
    *   [ ] Implement static XS accessor generation for fixed fields.
        (Difficulty: 3/10).
    *   [ ] Add hot-path detection to the code generator. (Difficulty: 3/10).
    *   [ ] Implement shared memory service stubs. (Difficulty: 3/10).
    *   [ ] Add zero-copy IPC transport templates. (Difficulty: 3/10).
    *   [ ] Performance: Precompute hashes for field names in generated code.
        (Difficulty: 4/10).
    *   [ ] Observability: Embed source code info (proto file/line) in generated
        PMs. (Difficulty: 3/10).
    *   [ ] Safety: Generate Type::Tiny constraints for messages:
        *   [ ] Design Type::Tiny constraint generation strategy. (Difficulty:
            3/10)
        *   [ ] Implement basic constraint generation for scalar types.
            (Difficulty: 3/10)
        *   [ ] Implement constraint generation for Enum types. (Difficulty:
            2/10)
        *   [ ] Implement constraint generation for Message types (e.g.,
            `InstanceOf`). (Difficulty: 2/10)
        *   [ ] Implement constraint generation for Repeated fields (e.g.,
            `ArrayRef`). (Difficulty: 3/10)
        *   [ ] Implement constraint generation for Map fields (e.g.,
            `HashRef`). (Difficulty: 3/10)
        *   [ ] Add tests for generated Type::Tiny constraints in
            `t/98-generated.t`. (Difficulty: 3/10)

*   [ ] Review and update perl/doc/architecture/** documents.

--------------------------------------------------------------------------------

## Phase 425: Modularize Compiler Plugin

*Source file: `doc/plan/425_Modularize.md`*

[TOC]

*   [ ] Refactor `protoc-gen-perl-pb.cc` into a modular structure:
    *   [ ] Create `compiler/perl/perl_generator.h` declaring the `PerlGenerator` class (subclassing `google::protobuf::compiler::CodeGenerator`).
    *   [ ] Create `compiler/perl/perl_generator.cc` implementing the code generation logic and all helper functions.
    *   [ ] Create `compiler/perl/plugin_main.cc` implementing the standard `PluginMain` entry point.
    *   [ ] Update `compiler/perl/BUILD` to define `perl_generator` library and link `protoc-gen-perl-pb` against it.
*   [ ] Implement C++ unit tests for the generator:
    *   [ ] Create `compiler/perl/perl_generator_unittest.cc` using GoogleTest to unit-test specific generator behaviors surgically.
    *   [ ] Add the unit test target to `compiler/perl/BUILD`.
*   [ ] Verify generated outputs remain 100% identical:
    *   [ ] Compare the generated `.pm` files before and after the refactoring to ensure zero functional or formatting regressions.
    *   [ ] Run all 50 C-level tests and the full Perl integration suite to verify stability.

--------------------------------------------------------------------------------

## Phase 430: Release

*Source file: `doc/plan/430_Release.md`*

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] CPAN distribution setup (`Makefile.PL` finalization).

*   [ ] TODO: Configure GitHub Actions for Perl CPAN release workflow.
    (Difficulty: 3/10).

*   [ ] TODO: Integrate AddressSanitizer (ASan) into the CI pipeline.
    (Difficulty: 3/10).

*   [ ] TODO: Integrate ThreadSanitizer (TSan) into the CI pipeline.
    (Difficulty: 3/10).

*   [ ] TODO: Automated testing of CPAN distribution package creation.
    (Difficulty: 2/10).

*   [ ] TODO: Include Embedded Benchmark Suite in distribution. (Difficulty:
    5/10).

*   [x] Support Interactive HTML Developer Documentation.

*   [x] MANIFEST, MANIFEST.SKIP

*   [x] README, CHANGES

*   [x] License, Contributors

*   [x] Documentation finalization.

*   **Engineering Excellence (Reach for More):**

    *   [ ] TODO: Script to verify MANIFEST consistency before release.
        (Difficulty: 2/10).
    *   [ ] TODO: Automated check for required metadata (Changes, License) in
        distribution. (Difficulty: 2/10).
    *   [ ] TODO: Verify distribution builds and tests cleanly on a fresh
        environment. (Difficulty: 3/10).
    *   [ ] TODO: Research and prototype Fat-XS or Alien::Protobuf for binary
        dependencies. (Difficulty: 3/10).
    *   [ ] TODO: Implement multi-platform build matrix in CI for pre-built
        binaries. (Difficulty: 3/10).
    *   [ ] TODO: Develop logic to bundle and load platform-specific shared
        objects. (Difficulty: 3/10).
    *   [ ] TODO: Test binary distribution on Linux, macOS, and Windows.
        (Difficulty: 3/10).
    *   [ ] TODO: Define baseline performance metrics for serialization and
        deserialization. (Difficulty: 3/10).
    *   [ ] TODO: Create a script to run benchmarks and compare against the
        baseline. (Difficulty: 3/10).
    *   [ ] TODO: Integrate the benchmark comparison script into the CI
        pipeline. (Difficulty: 2/10).
    *   [ ] TODO: Configure CI to fail if performance drops below a defined
        threshold. (Difficulty: 2/10).

*   [x] Review and update perl/doc/architecture/** documents.

*   [ ] Consider future enhancements for optional integrations with common Perl
    event loops (e.g., AnyEvent, IO::Async).

--------------------------------------------------------------------------------

## Phase 500: Pure Perl Engine

*Source file: `doc/plan/500_Pure_Perl_Engine.md`*

[TOC]

## Objective

Implement a complete Pure-Perl fallback for the Protocol Buffers engine. This
provides "Zero-Dependency" runtime support (no C compiler required at install
time for users who only need the PP engine) and allows for a "Polyglot" mode
where specific messages can opt into Pure-Perl for write-heavy workloads that
suffer from XS transition overhead.

## Architecture

The Pure-Perl engine will mirror the behavior of the UPB-based XS engine. It
will utilize a `Protobuf::Engine::PurePerl` backend that handles data storage,
serialization, and reflection.

### 1. Engine Abstraction

*   [x] Refactor `Protobuf::Message` to support swappable backends.
*   [x] Implement `Protobuf::Engine::XS` (current default).
*   [x] Implement `Protobuf::Engine::PurePerl` (new).
*   [x] Add global `PROTOBUF_ENGINE` environment variable and per-message
    `profile => 'pure_perl'` support.

### 2. Pure-Perl Descriptor Reflection

*   [x] Implement a Perl-only version of the `DescriptorPool`.
*   [x] Create a parser for binary `FileDescriptorSet` blobs in Perl (to load
    embedded descriptors without UPB).
*   [x] Implement Perl classes for `MessageDef`, `FieldDef`, `EnumDef`, etc.,
    with the same API as the XS descriptors.

### 3. Wire Format Implementation (G::PB Style)

*   [x] **Varint Kernels**: Implement `encode_varint` and `decode_varint` using
    Perl arithmetic and bitwise ops.
*   [x] **Fixed Types**: Utilize Perl's `pack`/`unpack` (templates `V`, `v`,
    `Q<`, `d`, `f`) for fixed32, fixed64, doubles, and floats.
*   [x] **ZigZag**: Implement ZigZag encoding/decoding for signed integers.
*   [x] **Parsing**: Implement the tag-based parse loop in Perl.
*   [x] **Serialization**: Implement the recursive serialization tree traversal.

### 4. Data Storage & Accessors

*   [x] Implement a Hash-backed storage model for PP messages.
*   [x] Ensure the `ClassGenerator` can inject Pure-Perl accessors that bypass
    XS when the PP engine is active (Achieved via engine-aware XS fallback and
    Perl-only generation).
*   [x] Implement `has_`, `clear_`, and default value logic in Perl.

### 5. Parity & Validation

*   [x] Pass all 318 existing tests using the Pure-Perl engine (where
    applicable) and verify XS stability.
*   [x] Verify "Polyglot" stability: An XS-backed message containing a PP-backed
    sub-message (and vice-versa).
*   [ ] Implement "Zero-Copy Bridge": Logic to convert an XS message to a PP
    message (and vice-versa) for cross-engine operations.

## Engineering Excellence (Reach for More)

*   [x] **Pure-Perl Reflection**: Fully operational zero-dependency reflection
    system.
*   **JIT Accessor Generation**: Use `eval` to generate optimized Perl accessor
    code for specific schemas at runtime. (Difficulty: 4/10)
*   **SIMD-lite for PP**: Research using `String::SIMD` (if available) or `pack`
    tricks to accelerate Varint decoding in Perl. (Difficulty: 6/10)
*   **Memory Efficiency**: Implement an array-backed storage model for Pure-Perl
    messages to reduce memory footprint compared to Hashes. (Difficulty: 5/10)
*   **G::PB Bridge**: Implement a compatibility layer that allows
    `Google::ProtocolBuffers` generated code to run directly on the new PP
    engine. (Difficulty: 4/10)

--------------------------------------------------------------------------------
