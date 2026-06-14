# Architecture

[TOC]

<!-- Section: 000-Architecture-Overview.md -->

## Perl Protobuf Architecture Overview

### 1. Overall Philosophy

This project implements a high-performance Perl extension for Protocol Buffers,
leveraging the core `upb` C library. Key principles include:

*   **UPB-centric:** The `upb` library is the foundation for all C-level
    operations.
*   **Performance:** Designed for speed, using XS to bridge Perl and C,
    minimizing Perl-level overhead.
*   **Modern Perl:** Utilizes Moo for the object system in the Perl layer.
*   **Dynamic Class Generation:** Perl classes representing message types are
    generated dynamically at runtime when descriptors are loaded.
*   **Memory Safety:** Emphasizes arena-based memory management from `upb` and
    careful lifecycle management between Perl objects and C structures.
*   **Comprehensive Testing:** Includes both Perl-level unit/integration tests
    and extensive C-level tests.
*   **Python Parity:** Aims for feature and behavioral parity with the official
    Python UPB-based implementation.

### 2. Directory Structure (`perl/`)

*   **`lib/`**: Contains the Perl modules (`.pm` files) forming the public API
    (e.g., `Protobuf.pm`, `Protobuf/Message.pm`, `Protobuf/DescriptorPool.pm`,
    etc.) and their associated `.xs` files for XS bindings.
*   **`xs/`**: Contains pure C helper functions and implementations for various
    components (e.g., `convert`, `descriptor`, `message`, `protobuf` core
    utilities like arena and object cache). These are compiled into
    `libprotobufperl.so`.
*   **`t/`**: Perl-level test files (`.t`).
    *   **`t/c/`**: C-level test files (`.c`). These are compiled into
        standalone executables.
    *   **`t/integration/`**: Perl tests focusing on the interaction of multiple
        components.
    *   **`t/lib/`**: Helper modules for Perl tests.
*   **`xt/`**: Extended tests, including author tests, release tests, and
    concurrency tests for Coro, Mojo, etc.
*   **`inc/`**: Modules and templates used by the build system (`Makefile.PL`,
    `BUILD.bazel.PL`).
*   **`local/`**: Vendored Perl dependencies for the build system.
*   **`typemap`**: Contains the global typemap file for XS.
*   **`doc/`**: Project documentation, including plans and architecture.
*   **`protoc/`**: Source for the `protoc-gen-perl-pb` plugin.
*   **`vendor/`**: JIT-populated at build time. Contains the `upb` library,
    `utf8_range`, and dynamically fetched `libcoro` sources. None of these are
    checked into the repository to ensure license compliance and prevent code
    duplication.

### 3. Build Systems

*   **`Makefile.PL`**: The primary build system using `ExtUtils::MakeMaker`. It
    triggers `maint/vendor_upb.pl` to populate the `vendor/` directory, and then
    compiles the JIT-vendored `upb` and `utf8_range` sources from `vendor/`,
    along with custom XS helper code from `xs/`, into a central
    `libprotobufperl.so`. It also manages XS compilation and C test compilation.
    Template Toolkit is used within `MY::` overrides to generate complex
    Makefile rules from `.mk.tt` files in `inc/templates/`.
*   **`BUILD.bazel.PL`**: A script to generate `perl/BUILD.bazel`, integrating
    the Perl components with the main project's Bazel build. This is currently
    under development and aims to build `libprotobufperl`, C tests, and
    benchmarks.

### 4. Core Components

*   **`libprotobufperl.so`**: A shared C library built by `Makefile.PL`,
    containing the compiled UPB sources, common C utilities from `protobuf/src`,
    and custom C code from `perl/xs/`. This is linked by all XS modules.
*   **`Protobuf::*` Modules (`lib/`)**: The user-facing Perl classes.
*   **XS Bindings (`lib/Protobuf/*.xs`, `xs/`)**: The bridge between Perl and
    the C functions in `libprotobufperl.so`.
*   **Object Cache (`xs/protobuf/obj_cache.c`)**: Ensures that the same C
    pointer maps to the same Perl object, crucial for object identity. Uses a
    striped mutex design for concurrency.
*   **Arena Management (`xs/protobuf/arena.c`)**: Wraps `upb_Arena` for memory
    management.
*   **Type Conversion (`xs/convert/`)**: Handles marshalling between Perl SVs
    and UPB C types.

### 5. Testing Strategy

*   **C-Level Tests (`t/c/`)**: Standalone C programs testing the C API of
    `libprotobufperl` and interactions with UPB. Uses a custom harness
    (`t/c/upb-perl-test.h`).
*   **Perl Unit Tests (`t/`)**: Testing the Perl API of `Protobuf::*` modules.
*   **Integration Tests (`t/integration/`)**: Testing how different Perl modules
    interact.
*   **Concurrency Tests (`xt/coro/`, `xt/mojo/`)**: Testing thread and event
    loop safety.
*   **Sanitizers**: Encourages use of ASan and TSan during development.

### 6. Key Architectural Decisions & Features

*   **Dynamic Class Generation**: `Protobuf::ClassGenerator` creates Perl
    classes for messages on the fly when descriptors are loaded.
*   **Tied Interfaces**: `Protobuf::Internal::Repeated` and
    `Protobuf::Internal::Map` use `Tie::Array` and `Tie::Hash` to provide
    idiomatic Perl interfaces to the underlying `upb_Array` and `upb_Map`.
*   **Per-Interpreter Registry**: Centralized C struct to hold per-interpreter
    state like the object cache, minimizing global lookups.
*   **Advanced Memory Management**: Features like tmpfs-backed arenas for IPC,
    memory canaries, and block allocators are implemented.
*   **Engineering Excellence Goals**: The project has numerous "Engineering
    Excellence" goals outlined in the plan documents, aiming for very high
    performance, robustness, and advanced features (e.g., SIMD, NUMA awareness,
    fuzzing, guard pages).

### 7. Relationship to Main Protobuf Project

*   The Perl build system JIT-vendors C sources from the parent `upb` and `src`
    workspaces (or `blaze-bin` in Google3) into a local `vendor/` directory at
    build time, ensuring perfect alignment with the canonical Protobuf version.
*   The XS layer often mirrors the structure and logic of the CPython UPB
    extension.
*   Aims to provide a Perl interface that is consistent with other languages.

See the other documents in this `architecture/` directory for more details on
specific aspects.

--------------------------------------------------------------------------------

<!-- Section: 020-Perl-API-Design.md -->

## Architecture: Perl API Design

### Object-Oriented System

1.  **Moo:** The Perl modules will be implemented using **Moo**. This provides a
    lightweight, modern OO system with good performance and a gentle learning
    curve.
2.  **Immutability:** Where appropriate, attributes should be read-only (`ro`).
3.  **Type Constraints:** Moo's type constraints should be used for attribute
    validation where beneficial.
4.  **Type::Tiny Integration**: (Planned) Seamless integration with `Type::Tiny`
    for rigorous message and field validation, including automatic generation of
    type libraries for every Protobuf package.
5.  **Compiled Validation**: (Planned) High-performance C-level validation logic
    is exported as compiled Perl subroutines, allowing `Type::Tiny` to perform
    complex checks without full object inflation.
6.  **Intelligent Coercion**: (Planned) Support for deep, intelligent coercion
    of complex Perl `HashRefs` into nested Protobuf message trees, including
    automated `oneof` branch selection.

### Naming Conventions

1.  **Methods:** snake_case (e.g., `get_field_value`, `serialize_to_string`).
2.  **Attributes:** snake_case.
3.  **Package Names:** CamelCase (e.g., `Protobuf::Message`).

### Method Styles

1.  **Accessors:** Use standard Moo `has` for attribute access.
2.  **Actions:** Methods performing actions should be named descriptively (e.g.,
    `parse`, `serialize`, `add_item`).

### Future Considerations

1.  **Native Booleans:** Perl has added a native boolean type. Consider how this
    might be integrated into the API for boolean fields, potentially using
    `boolean.pm` or similar, for stricter type checking and clarity, instead of
    relying solely on Perl's standard truthiness.
2.  **Asynchronous Operations:** Explore integration with `IO::Async` or other
    event loops like `Mojo::IOLoop` to provide non-blocking APIs for operations
    like service calls or streaming, especially relevant for network-bound
    Protobuf usage.

--------------------------------------------------------------------------------

<!-- Section: ../internal/development_cycle.md -->

## Perl XS Development Cycle for Protobuf Module

This document outlines the typical iterative development and testing cycle used
for the `Protobuf` Perl module, which involves both Perl `.pm` files and C/XS
`.xs` files.

1.  **Modify Code:** Make changes to the `.pm` files (e.g.,
    `lib/Protobuf/Message.pm`), `.xs` files (e.g., `lib/Protobuf/Message.xs`),
    or the `typemap`.

2.  **Regenerate Makefile (if needed):** If you add new `.xs` files or change
    dependencies in `Makefile.PL`, you MUST re-run `perl Makefile.PL`. This is
    generally safe to do even if not strictly necessary.

3.  **Build:** Compile the changes. This updates the compiled components in the
    `blib/` directory.

    ```bash
    make
    ```

    *(Output is redirected to temporary files to reduce noise).*

    # To rebuild the common library used by C tests:

    make libprotobuf_common.a

4.  **Run Specific Tests:** Run the test file(s) most relevant to the changes
    made. The `-b` flag tells `prove` to use the modules in `blib/`. The `-v`
    flag provides verbose output.

    ```bash
    prove -bv t/05_message.t
    ```

5.  **Debug & Iterate:** If tests fail:

    *   Analyze the error messages and TAP output.
    *   Inspect code in `.pm` or `.xs` files.
    *   Add temporary `warn` statements or use `Data::Dumper` in Perl, or
        `fprintf(stderr, ...)` in XS for debugging.
    *   Re-run `make` and the specific `prove` command after fixes.
    *   For C tests in `t/c/`, you may need to force a re-link after changes:

        ```bash
        rm -f t/c/your_test && make t/c/your_test
        ```

6.  **Memory Check (if needed):** If a crash occurs (like a SEGV) or memory
    issues are suspected, run the relevant test under `Test::Valgrind` by
    ensuring `Test::Valgrind` is loaded in the `.t` file.

    ```bash
    prove -bv t/04_descriptor_pool.t
    ```

    Then analyze the `.err` output file specified in the `run_shell_command` for
    Valgrind reports.

7.  **Full Test Suite:** Once the specific tests pass, run the entire test suite
    to check for regressions:

    ```bash
    make test
    ```

8.  **Clean Build (Optional):** If encountering strange issues, a clean build
    can help:

    ```bash
    make realclean
    perl Makefile.PL
    make
    make test
    ```

This cycle ensures that changes to both Perl and XS code are compiled and tested
effectively. Frequent rebuilding with `make` is crucial after any code
modification before running tests.

--------------------------------------------------------------------------------

<!-- Section: 040-Testing.md -->

## Architecture: Testing

### C-Level Tests

1.  **Harness:** Custom C test harness in `t/c/upb-perl-test.h`.
2.  **Concurrency:** Use `libcoro` to stress test for re-entrancy and
    thread-safety issues in the C layer.

### Perl-Level Tests

1.  **Test::More:** Standard testing with `Test::More`.
2.  **Test::LeakTrace:** Mandated for all Perl modules wrapping C components to
    detect memory leaks.
3.  **Test::Exception:** For testing exception-based error handling.
4.  **Devel::Cover:** Aim for >= 90% statement, branch, and condition coverage
    for all Perl modules. Coverage reports should be generated during the test
    process.

--------------------------------------------------------------------------------

<!-- Section: testing/01-xs-testing.md -->

## XS Testing Strategy

*Status: Partially Implemented*

Testing the XS layer involves a combination of Perl-level tests and dedicated
C-level tests.

### 1. Perl-level Tests (`.t` files)

-   Primary method for testing the XS code through the Perl API.
-   Located in the `t/` directory.
-   Use `Test::More` and other testing modules.
-   **Mocking:** `Test::MockModule` is used to isolate testing of modules like
    `Protobuf::Descriptor` before their dependencies
    (`Protobuf::DescriptorPool`) are fully implemented.

### 2. C-level Assertions/Checks

-   `assert()` in XS code for development-time sanity checks.
-   Runtime checks of `upb` function return values, propagating errors via
    `croak`.

### 3. Dedicated C Tests for UPB Interaction (`t/c/`)

-   **Purpose:** Essential for debugging complex interactions with the `upb`
    library, especially when SEGVs or other crashes occur that are hard to
    diagnose from the Perl side.
*   **Method:** Small, standalone C programs (e.g., `t/c/protobuf_obj_cache.c`,
    `t/c/convert/upb_to_sv.c`) that directly call functions from `xs/protobuf.h`
    and the `upb` library, mimicking the XS logic.
-   **Benefits:**
    -   Isolation of C-level issues.
    -   Reliable `fprintf` debugging.
    -   Independent compilation and execution.
*   **Build:** C tests are compiled by rules added to the `MY::postamble`
    section of `perl/Makefile.PL`. Each test must have an entry in the
    `$c_test_config` hash in `perl/t/c/c_test_config.json` to ensure it's linked
    with appropriate flags from `ExtUtils::Embed` and against
    `libprotobuf_common.a`.

    *   **Example `c_test_config.json` entry:**

        ```json
        "convert/upb_to_sv": {
            "deps": ["xs/protobuf.h", "t/c/upb-perl-test.h", "xs/convert.h"],
            "extra_src": [
                "t/c/convert/test_util.c",
                "t/c/convert/types/int32.c",
                "t/c/convert/types/string.c"
                // ... and so on for other types
            ]
        }
        ```

    *   **Rebuilding:** To ensure changes are picked up, you might need to
        remove the test binary (`rm -f t/c/convert/upb_to_sv`) before running
        `make t/c/convert/upb_to_sv`.

*   **Test Harness:** `t/c/upb-perl-test.h` provides TAP-compliant macros:

    -   `plan(n)`: Set the expected number of tests.
    -   `ok(val, name)`: Basic boolean check.
    -   `is(got, expected, name)`: Integer comparison.
    -   `is_string(got, expected, name)`: String comparison.
    -   `TODO("reason") { ... }`: Mark tests as deferred (Schwern-style).
    -   `SKIP("reason", count)`: Skip a block of tests.
    -   `subtest("name", { ... })`: Indented subtest support for hierarchical
        organization.
    -   `LEAK_CHECK(arena, { ... }, "name")`: Verifies local allocation
        neutrality using arena-level statistics.

*   **Concurrency Stress:** Tests in `t/c/integration/` MUST use the
    `STRESS_THREADS(n, func)` or `libcoro` patterns to verify re-entrancy and
    thread-local state safety.

*   **Engineering Excellence (Reach for More):**

    -   **Chaos Allocation Engine:** Future tests will utilize a pluggable C
        allocator to randomly inject `malloc` failures, verifying the robustness
        of error propagation and resource cleanup.
    -   **SIMD-Aware Instruction Coverage:** Implementation of reporting tools
        to ensure VPP-style vectorized paths (SSE4.1/AVX2) are comprehensively
        exercised.
    -   **Binary-Diff Serialization:** Use of canonical golden binaries to
        verify bit-exact stability of serialization output across versions.

*   **Memory Safety:** Verification under AddressSanitizer (ASan) is mandatory.

*   **Embedding Perl:** The C tests embed a minimal Perl interpreter to use
    basic SV manipulation functions. This is done via `PERL_SYS_INIT3`,
    `perl_alloc`, `perl_construct`, and `perl_parse` with a minimal script like
    `"-e", "0"`. `perl_destruct` and `perl_free` are called at the end.

*   **ERRSV Content:** Due to the minimal embedded interpreter, reliably
    asserting the exact string content of `ERRSV` after a `croak` is difficult,
    as other operations within the embedded Perl might alter `ERRSV`. C tests
    should primarily focus on confirming that a croak *occurred* (e.g., by
    checking the non-zero return from `JMPENV_PUSH`), rather than matching the
    exact error message. Detailed error message testing is best done in the
    Perl-level `.t` tests, which have a full `Test::More` environment.

### 3.1. Type Conversion Tests (`t/c/convert/`)

-   To keep things organized, tests for type conversions (e.g.,
    `PerlUpb_UpbToSv` and `PerlUpb_SvToUpb`) are further structured.
-   **`t/c/convert/types/`:** This subdirectory contains individual `.c` files,
    each dedicated to testing a specific `upb_FieldType` (e.g., `int32.c`,
    `string.c`, `message.c`). These files define:
    -   Functions to set up test values of that type.
    -   Functions to check the converted Perl SVs.
    -   Data structures or functions that contribute to the main test cases
        array.
-   **`t/c/convert/upb_to_sv.c`:** This C file acts as the main test runner for
    UPB-to-SV conversions. It includes headers or declarations from the `types/`
    files and iterates through the test cases.
-   **`t/c/convert/sv_to_upb.c`:** Similarly, this runs the SV-to-UPB conversion
    tests, also utilizing the logic from the `types/` directory.
-   **`t/c/convert/test_util.c`:** Contains common helper functions used by both
    `upb_to_sv.c` and `sv_to_upb.c`, such as loading descriptors.

See [Type-Specific C Tests](#type-specific-c-tests) for more details on the
structure of files within `t/c/convert/types/`.

\

### 4. Debugging Segmentation Faults

-   **`fprintf` to stderr:** Liberal use of `fprintf(stderr, "DEBUG: ...\\n");`
    is the first line of defense. Print variable values, pointers, and progress
    markers before and after any call to `upb` functions or complex logic,
    especially in code paths leading to a crash.
-   **AddressSanitizer (ASan):** If `fprintf` isn't enough or a stack corruption
    is suspected, compile with ASan. Modify `perl/Makefile.PL`:

    ```perl
    # In Makefile.PL
    WriteMakefile(
        # ... other options ...
        'CCFLAGS' => "-g $Config{ccflags} -fPIC -fsanitize=address",
        'LDDLFLAGS' => "$Config{lddlflags} -fsanitize=address",
    );
    ```

    Then rebuild cleanly:

    ```bash
    cd perl
    make clean
    perl Makefile.PL
    make -j$(nproc)
    # Run the specific C test that crashes
    make -j$(nproc) test_c
    ```

    ASan will print a report to stderr if it detects memory errors.

-   **GDB:** For crashes that occur *during* a function call, use GDB to step
    through the assembly instructions (`stepi`) to pinpoint the exact failure
    point.

-   **Isolate the Failure:** When a test in `t/c/convert/upb_to_sv.c` (or
    similar) fails, check the setup functions in the corresponding
    `t/c/convert/types/foo.c` file. Errors in test data setup (like using the
    wrong message name in `upb_DefPool_FindMessageByName`) are common.
    Instrument these setup functions heavily.

-   **Double-Free Errors:** Segfaults, especially at the end of a test run, can
    indicate a double-free. Carefully check resource management in the test's
    `main` function. Ensure arenas, pools, and any manually allocated memory are
    freed exactly once.

-   **Verify String Literals:** Typos in string literals used for lookups (e.g.,
    field names, message names in `upb_DefPool_FindMessageByName`) can lead to
    null pointers and crashes. Double-check these strings.

### 5. Memory Leak Detection

-   Perl's refcounting and `upb` arenas should manage most memory.
-   `Test::Valgrind` is included in `TEST_REQUIRES` to help detect leaks.

    ```bash
    valgrind --leak-check=full perl Makefile.PL && make && make test
    ```

### 5. Porting Tests from Python

-   Review the test cases in the Python implementation
    (`python/google/protobuf/internal/*_test.py`) and port relevant scenarios to
    Perl `.t` files. This will help ensure feature parity and catch edge cases
    already coverd in the Python version.

### 6. C-Level Coroutine Safety Testing (`t/c/integration/*_coro.c`)

-   To test the re-entrancy and safety of C functions when used in a concurrent
    manner, we use the `libcoro` C library (vendored in `perl/libcoro/`).
-   Tests are created in `t/c/integration/` (e.g., `030_protobuf_coro.c`).
-   These C tests *do not* use the Perl `Coro` module. Instead, they use
    `libcoro` to create and manage C-level coroutines.
-   A dummy Perl interpreter context is generally faked using macros, as the
    primary goal is to stress the C code's interaction with shared resources
    like the object cache, not the Perl-C API calls themselves.
-   The `Makefile.PL` adds `libcoro/coro.c` to the `extra_src` for these
    specific test targets in the `$c_test_config` hash.
-   This allows early detection of C-level race conditions or state corruption
    issues.

### 7. Perl-Level Coroutine Safety Testing (`xt/author/coro-safe.t`)

-   This test uses the actual Perl `Coro` module.
-   It calls the *Perl API* of the `Protobuf::*` modules from within `async`
    blocks.
-   This test verifies the safety of the XS glue code when used in a real Perl
    Coro environment.

--------------------------------------------------------------------------------

<!-- Section: testing/02-type-testing.md -->

## Type-Specific C Tests

*Status: Planned*

To make the C-level tests for type conversions more modular and maintainable,
the test logic for each individual `upb_FieldType` is encapsulated in its own
file within `perl/t/c/convert/types/`.

### File Structure (`t/c/convert/types/<type>.c`)

Each `<type>.c` file (e.g., `int32.c`, `string.c`, `message.c`) should generally
contain:

1.  **Setter Function(s):** Functions like `set_<type>_values(upb_MessageValue
    *val, ...)` to populate a `upb_MessageValue` with test data for this type.
2.  **Checker Function(s):** Functions like `check_sv_is_<type>(pTHX_ SV *sv,
    ...)` to assert that the Perl SV resulting from a conversion matches the
    expected value and type. These functions use the macros from
    `t/c/upb-perl-test.h` to print TAP output.
3.  **Test Case Definitions:** A static array of structs, or functions returning
    such, that define specific test scenarios for this type. These scenarios are
    then used by the main test runners (`upb_to_sv.c`, `sv_to_upb.c`).

### Example: `int32.c` (Conceptual)

```c
##include "t/c/convert/test_util.h"
##include "t/c/upb-perl-test.h"
##include "xs/convert/upb_to_sv.h" // Or sv_to_upb.h

// test_num is external from the main runner
extern int test_num;

static void set_int32_123(upb_MessageValue *val) { val->int32_val = 123; }

static void check_sv_int32_123(pTHX_ SV *sv, const char *prefix) {
    ok(SvIOK(sv), sdiagnostic("%s: SV is IOK", prefix));
    is(SvIV(sv), 123, sdiagnostic("%s: SV value correct", prefix));
}

const upb_to_sv_test_case int32_test_cases[] = {
    {"optional_int32", "int32", kUpb_FieldType_Int32, set_int32_123, check_sv_int32_123, 2},
    // ... more int32 test cases ...
    {NULL}
};
```

### Integration

The main test files (`t/c/convert/upb_to_sv.c` and `t/c/convert/sv_to_upb.c`)
include the declarations from these type files and calculate the `total_tests`
dynamically by iterating through the `*_test_cases` arrays using
`count_test_cases()`.

Edge cases, such as handling `NULL` values or `undef` SVs, are tested as
independent blocks within the main runners to ensure exhaustive coverage of the
conversion API's boundary conditions.

--------------------------------------------------------------------------------

<!-- Section: 050-Benchmarking.md -->

## Architecture: Benchmarking

1.  **Framework:** Use `Benchmark::Dumb` for simplicity and low overhead.
2.  **Targets:** Key operations to benchmark:
    *   Message creation.
    *   Field access (set/get for various types).
    *   Serialization (to binary string).
    *   Deserialization (from binary string).
    *   TextFormat and JSON en/decoding.
3.  **Comparisons:** Where feasible, compare against:
    *   `Google::ProtocolBuffers`
    *   `Protobuf::XS`
4.  **Reports:** Benchmarking results should be logged and summarized.

--------------------------------------------------------------------------------

<!-- Section: 060-C-Test-Harness.md -->

## C Test Harness Architecture

### Overview

The C-level test harness (`perl/t/c/upb-perl-test.h` and `.c`) provides macros
and functions to facilitate testing the C/XS components of the `Protobuf`
module. It aims to produce TAP 13 compliant output.

### Implemented Macros

See `perl/doc/plan/010_Test.md` for a list of implemented macros.

### Engineering Excellence Goals

-   **Test::Deep Style Comparisons:** Implement macros for deep comparison of C
    data structures, similar to Perl's `Test::Deep`.
-   **Property-Based Testing:** Integrate hooks for property-based testing,
    allowing for random data generation and shrinking to find minimal failing
    cases.
-   **Benchmark Driven TODOs:** Automatically analyze benchmark results and
    generate `TODO` stubs in test files for areas needing performance-related
    tests.
-   **Harness Fuzzing:** Fuzz the inputs to the test macros themselves to ensure
    the harness is robust.
-   **TAP Protocol Conformance:** rigorously test the harness output against the
    TAP 13 specification.
-   **Macro Unit Tests:** Develop a dedicated test suite for the macros defined
    in `upb-perl-test.h`.

--------------------------------------------------------------------------------

<!-- Section: 080-C-Benchmark-Format.md -->

## C Benchmark Output Format

To enable automatic analysis and TODO generation, C benchmarks run via `make
bench` should output results in a machine-readable format.

Each benchmark run should output a single line of JSON to STDOUT.

### Fields:

-   `suite`: (String) The name of the benchmark suite.
-   `name`: (String) The specific name of the benchmark test.
-   `iterations`: (Integer) The number of iterations performed.
-   `time_ns`: (Integer) Total time taken in nanoseconds.
-   `bytes_per_sec`: (Float, Optional) Throughput in bytes per second, if
    applicable.
-   `ops_per_sec`: (Float, Optional) Throughput in operations per second, if
    applicable.
-   `error`: (String, Optional) Any error message encountered during the
    benchmark.

### Example:

```json
{"suite": "ArenaBench", "name": "AllocFree", "iterations": 1000000, "time_ns": 50000000, "ops_per_sec": 20000000}
{"suite": "MessageBench", "name": "ParseSmall", "iterations": 10000, "time_ns": 120000000, "bytes_per_sec": 85000000}
```

This format can be easily parsed by scripts to compare against baselines or
thresholds.

--------------------------------------------------------------------------------

<!-- Section: 060-Documentation.md -->

## Architecture: Documentation

1.  **POD:** All Perl modules MUST have comprehensive documentation in Plain Old
    Documentation (POD) format.
2.  **CAPI:** Public C functions intended for direct or indirect use by XS MUST
    be clearly documented in the header files.
3.  **Metacpan:** Ensure documentation renders correctly on Metacpan.
4.  **Examples:** Include usage examples in the POD.
5.  **Generation:** No special tools required beyond standard Perl POD tooling.

--------------------------------------------------------------------------------

<!-- Section: build/01-upb-integration.md -->

## UPB Build & JIT Vendoring Integration

The Perl `Protobuf` module integrates with the `upb` C library, compiling it as
part of the XS extension build process using `ExtUtils::MakeMaker`.

To ensure a clean licensing model (strictly Apache 2.0 for checked-in code) and
prevent version skew, the project utilizes a **JIT (Just-In-Time) Vendoring**
architecture.

--------------------------------------------------------------------------------

### 1. JIT Vendoring Architecture

Rather than checking the `upb` and `libcoro` source code directly into the
repository, the build system dynamically populates a local `vendor/` directory
at build time.

The process is orchestrated by
[maint/vendor_upb.pl](file:///google/src/cloud/cjac/cjac-protobuf-20260606/google3/third_party/protobuf/perl/maint/vendor_upb.pl)
and is automatically triggered by
[Makefile.PL](file:///google/src/cloud/cjac/cjac-protobuf-20260606/google3/third_party/protobuf/perl/Makefile.PL):

#### A. Open-Source Dev Mode

*   **`upb` & `utf8_range`**: Sourced locally from sibling directories in the
    monorepo/development workspace.
*   **`libcoro`**: Since `libcoro` is GPL/Gems Rx licensed, it is **never**
    checked in. The script dynamically fetches `coro.c`, `coro.h`, and `LICENSE`
    from a stable GitHub mirror via `HTTP::Tiny` during the build, keeping the
    repository strictly Apache 2.0.

#### B. Google3 Dev Mode

In Google3, we must integrate with Google's internal `upb` and `proto2`
infrastructure:

*   **Sibling Sourcing**: `upb` and `utf8_range` are JIT-copied from sibling
    `//third_party/upb` and `//third_party/utf8_range` directories.
*   **Blaze Integration**: Specific descriptor headers and C sources (like
    `descriptor.upb_minitable.c` and `descriptor.upbdefs.c`) are compiled by
    Blaze and JIT-copied from `blaze-bin/` into the `vendor/` directory.
*   **LLVM Bitcode Bypass**: Blaze-built static archives (`.a`) contain LLVM
    bitcode (LTO) which is incompatible with the local host's standard GNU
    linker. To bypass this, we copy the **`.c` sources** from `blaze-bin` and
    compile them locally using the host's standard `gcc`, producing native ELF
    machine code that links perfectly.
*   **Compatibility Wrapper**: To bridge the gap between open-source types
    (e.g., `google_protobuf_FileDescriptorProto`) and Google3 types (e.g.,
    `proto2_FileDescriptorProto`), the JIT script generates a lightweight
    compatibility wrapper at
    `vendor/upb/upb/reflection/stage0/google/protobuf/descriptor.upb.h` that
    redirects to the Google3 descriptor and maps the symbols using preprocessor
    macros.

--------------------------------------------------------------------------------

### 2. Shared Library Compilation

To facilitate state sharing across multiple XS modules (e.g., ensuring a single,
unified Object Cache), the project compiles all `upb` and XS sources into a
central **shared library** (`libprotobufperl.$(DLEXT)`):

*   **Source Gathering**: `Makefile.PL` and `FileLists.pm` gather all `.c` files
    exclusively from the JIT-populated `vendor/` directory (e.g., `vendor/upb/`,
    `vendor/src/`, `vendor/third_party/`) and core XS logic in `xs/`.
*   **Linking**: Object files are compiled and linked into `libprotobufperl.so`.
*   **Visibility Control**: The build system utilizes a linker script
    ([libprotobufperl.map](file:///google/src/cloud/cjac/cjac-protobuf-20260606/google3/third_party/protobuf/perl/libprotobufperl.map))
    to restrict symbol visibility. In Google3, this script is configured to
    export `proto2_*` symbols in addition to `PerlUpb_*` and `upb_*` to ensure
    the C tests can resolve the Google3 descriptor symbols.
*   **Sanitizer Integration**: Supports ASan, UBSan, and MSan via `USE_ASAN=1`,
    `USE_UBSAN=1`, and `USE_MSAN=1` environment variables.

--------------------------------------------------------------------------------

### 3. Advantages

*   **Licensing Compliance**: Zero non-Apache code is checked into the
    repository.
*   **No Code Duplication**: Sourced directly from the canonical `upb`
    workspace.
*   **Google3 Hermeticity**: Integrates natively with Google3's generated
    descriptors and Blaze build system.
*   **Simplified User Build**: Standard Perl toolchain (`perl Makefile.PL &&
    make`) works out of the box for users.

--------------------------------------------------------------------------------

<!-- Section: build/02-xs-organization.md -->

## XS & C File Organization

*Status: Implemented*

The C and XS code for the `Protobuf` Perl module is highly structured,
separating user-facing XSUB bindings from the underlying C helper logic. It
leverages `XSMULTI => 1` in `Makefile.PL` to build multiple clean XS extensions
linked against a central shared library.

--------------------------------------------------------------------------------

### 1. XS Binding Files (`lib/Protobuf/`)

These files contain the `MODULE` and `PACKAGE` declarations, mapping Perl
package methods directly to C helper functions (XSUBs). They are kept minimal,
delegating all complex logic to the C library:

*   **`lib/Protobuf.xs`**: Main module initialization.
*   **`lib/Protobuf/Arena.xs`**: Bindings for `Protobuf::Arena` memory
    management.
*   **`lib/Protobuf/Descriptor.xs`**: Bindings for descriptor classes (e.g.,
    `FileDescriptor`, `MessageDef`).
*   **`lib/Protobuf/DescriptorPool.xs`**: Bindings for
    `Protobuf::DescriptorPool` (wrapping `upb_DefPool`).
*   **`lib/Protobuf/Message.xs`**: Bindings for `Protobuf::Message` instance
    operations (getters, setters, serialization).
*   **`lib/Protobuf/Repeated.xs`**: Bindings for `Protobuf::Internal::Repeated`
    container.
*   **`lib/Protobuf/Map.xs`**: Bindings for `Protobuf::Internal::Map` container.

--------------------------------------------------------------------------------

### 2. Shared C Library (`xs/`)

All core logic is implemented in pure C under the `xs/` directory. These files
are compiled and linked into `libprotobufperl.so`:

#### A. Core & Registry

*   **`xs/protobuf.c` / `.h`**: Global library initialization and registration.
*   **`xs/protobuf/registry.c` / `.h`**: Per-interpreter registry, managing
    thread-safe global state.
*   **`xs/protobuf/obj_cache.c` / `.h`**: Striped-mutex object cache, ensuring
    unique 1:1 mapping between C pointers and Perl SVs.
*   **`xs/protobuf/arena.c` / `.h`**: Custom `upb_Arena` lifecycle and
    allocation management.

#### B. Type Conversion

*   **`xs/convert.c` / `.h`**: Main entry point for marshalling.
*   **`xs/convert/sv_to_upb.c` / `.h`**: Deep conversion from Perl SVs (hashes,
    arrays, scalars) to UPB values.
*   **`xs/convert/upb_to_sv.c` / `.h`**: Deep conversion from UPB values to
    reified Perl SVs.

#### C. Descriptors & Pool

*   **`xs/descriptor_pool.c` / `.h`**: Registry and management of `upb_DefPool`.
*   **`xs/descriptor_pool/`**: Specific implementations for `add.c`, `find.c`,
    and `pool.c`.
*   **`xs/descriptor.c` / `.h`**: Common descriptor utilities.
*   **`xs/descriptor/`**: Type-specific C wrappers for `file.c`, `message.c`,
    `field.c`, `oneof.c`, `enum.c`, `enum_value.c`, `service.c`, and `method.c`.
*   **`xs/descriptor_containers.c` / `.h`**: C-level sequences and maps for
    accessing descriptor children.

#### D. Message Instances & Containers

*   **`xs/message.c` / `.h`**: Core message instance allocation and layout.
*   **`xs/message/`**: Specific implementations for `access.c` (reflection-based
    getters/setters), `compare.c` (equality), `serialize.c` (binary/text/JSON),
    and `meta.c`.
*   **`xs/repeated.c` / `.h`**: C-level backing for repeated fields.
*   **`xs/map.c` / `.h`**: C-level backing for map fields.
*   **`xs/extension_dict.c` / `.h`**: Custom dictionary for managing message
    extensions.

--------------------------------------------------------------------------------

<!-- Section: 080-Release-and-Lifecycle.md -->

## Release and Lifecycle Management

*Status: Planned*

This document outlines the strategy for packaging, distributing, and maintaining
the Protocol Buffers Perl implementation.

### CPAN Distribution

The project follows standard CPAN packaging conventions using
`ExtUtils::MakeMaker`.

-   **XS Integration**: The distribution includes a shared C library
    (`libprotobuf_common.so`) and multiple XS modules.
-   **Dependencies**: The primary dependency is the `upb` C library, which is
    bundled or linked depending on the environment.

### Advanced Release Goals

To achieve world-class software distribution and maintainability, the project
includes (or is planned to include) the following:

-   **Fully Automated CI/CD**: (Planned) Implement a GitHub Actions (or
    internal) pipeline that automatically runs the full test suite under
    AddressSanitizer (ASan) and ThreadSanitizer (TSan) for every commit and tag.
-   **Embedded Performance Benchmarks**: (Planned) Include a benchmarking suite
    in the distribution that users can execute to verify library performance on
    their specific hardware and Perl version.
-   **Searchable Architecture Documentation**: (Planned) Provide high-quality,
    searchable HTML documentation that integrates with MetaCPAN and provides
    deep insights into the C-layer and XS internals.
-   **Binary Distribution (Alien::upb)**: (Planned) Utilize the `Alien`
    namespace to provide pre-compiled versions of the `upb` library for common
    platforms, simplifying installation for users without a C compiler.

### Versioning

-   The project uses Semantic Versioning (SemVer).
-   Version alignment with the official Protocol Buffers release cycle will be
    maintained where possible.

--------------------------------------------------------------------------------
