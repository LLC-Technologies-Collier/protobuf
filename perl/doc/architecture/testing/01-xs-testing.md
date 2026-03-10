# XS Testing Strategy

_Status: Partially Implemented_

Testing the XS layer involves a combination of Perl-level tests and dedicated C-level tests.

## 1. Perl-level Tests (`.t` files)

-   Primary method for testing the XS code through the Perl API.
-   Located in the `t/` directory.
-   Use `Test::More` and other testing modules.
-   **Mocking:** `Test::MockModule` is used to isolate testing of modules like `Protobuf::Descriptor` before their dependencies (`Protobuf::DescriptorPool`) are fully implemented.

## 2. C-level Assertions/Checks

-   `assert()` in XS code for development-time sanity checks.
-   Runtime checks of `upb` function return values, propagating errors via `croak`.

## 3. Dedicated C Tests for UPB Interaction (`t/c/`)

-   **Purpose:** Essential for debugging complex interactions with the `upb` library, especially when SEGVs or other crashes occur that are hard to diagnose from the Perl side.
*   **Method:** Small, standalone C programs (e.g., `t/c/protobuf_obj_cache.c`, `t/c/convert/upb_to_sv.c`) that directly call functions from `xs/protobuf.h` and the `upb` library, mimicking the XS logic.
-   **Benefits:**
    -   Isolation of C-level issues.
    -   Reliable `fprintf` debugging.
    -   Independent compilation and execution.
*   **Build:** C tests are compiled by rules added to the `MY::postamble` section of `perl/Makefile.PL`. Each test must have an entry in the `$c_test_config` hash in `perl/t/c/c_test_config.json` to ensure it's linked with appropriate flags from `ExtUtils::Embed` and against `libprotobuf_common.a`.

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
    *   **Rebuilding:** To ensure changes are picked up, you might need to remove the test binary (`rm -f t/c/convert/upb_to_sv`) before running `make t/c/convert/upb_to_sv`.
*   **Test Harness:** `t/c/upb-perl-test.h` provides simple macros like `plan`, `ok`, `is`, `is_string`, `like_n`, and a `cdiag` function for formatted diagnostic output to stderr, suitable for C tests. These macros output TAP-compliant results. **Note:** These C tests run with a minimal embedded Perl interpreter and CANNOT load external modules like `Test::More` or its dependencies.
*   **Embedding Perl:** The C tests embed a minimal Perl interpreter to use basic SV manipulation functions. This is done via `PERL_SYS_INIT3`, `perl_alloc`, `perl_construct`, and `perl_parse` with a minimal script like `"-e", "0"`. `perl_destruct` and `perl_free` are called at the end.
*   **ERRSV Content:** Due to the minimal embedded interpreter, reliably asserting the exact string content of `ERRSV` after a `croak` is difficult, as other operations within the embedded Perl might alter `ERRSV`. C tests should primarily focus on confirming that a croak *occurred* (e.g., by checking the non-zero return from `JMPENV_PUSH`), rather than matching the exact error message. Detailed error message testing is best done in the Perl-level `.t` tests, which have a full `Test::More` environment.

## 3.1. Type Conversion Tests (`t/c/convert/`)

-   To keep things organized, tests for type conversions (e.g., `PerlUpb_UpbToSv` and `PerlUpb_SvToUpb`) are further structured.
-   **`t/c/convert/types/`:** This subdirectory contains individual `.c` files, each dedicated to testing a specific `upb_FieldType` (e.g., `int32.c`, `string.c`, `message.c`). These files define:
    -   Functions to set up test values of that type.
    -   Functions to check the converted Perl SVs.
    -   Data structures or functions that contribute to the main test cases array.
-   **`t/c/convert/upb_to_sv.c`:** This C file acts as the main test runner for UPB-to-SV conversions. It includes headers or declarations from the `types/` files and iterates through the test cases.
-   **`t/c/convert/sv_to_upb.c`:** Similarly, this runs the SV-to-UPB conversion tests, also utilizing the logic from the `types/` directory.
-   **`t/c/convert/test_util.c`:** Contains common helper functions used by both `upb_to_sv.c` and `sv_to_upb.c`, such as loading descriptors.

See [Type Testing](02-type-testing.md) for more details on the structure of files within `t/c/convert/types/`.

\
## 4. Debugging Segmentation Faults

-   **`fprintf` to stderr:** Liberal use of `fprintf(stderr, "DEBUG: ...\\n");` is the first line of defense. Print variable values, pointers, and progress markers before and after any call to `upb` functions or complex logic, especially in code paths leading to a crash.
-   **AddressSanitizer (ASan):** If `fprintf` isn't enough or a stack corruption is suspected, compile with ASan. Modify `perl/Makefile.PL`:
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
-   **GDB:** For crashes that occur *during* a function call, use GDB to step through the assembly instructions (`stepi`) to pinpoint the exact failure point.
-   **Isolate the Failure:** When a test in `t/c/convert/upb_to_sv.c` (or similar) fails, check the setup functions in the corresponding `t/c/convert/types/foo.c` file. Errors in test data setup (like using the wrong message name in `upb_DefPool_FindMessageByName`) are common. Instrument these setup functions heavily.
-   **Double-Free Errors:** Segfaults, especially at the end of a test run, can indicate a double-free. Carefully check resource management in the test's `main` function. Ensure arenas, pools, and any manually allocated memory are freed exactly once.
-   **Verify String Literals:** Typos in string literals used for lookups (e.g., field names, message names in `upb_DefPool_FindMessageByName`) can lead to null pointers and crashes. Double-check these strings.

## 5. Memory Leak Detection


-   Perl's refcounting and `upb` arenas should manage most memory.
-   `Test::Valgrind` is included in `TEST_REQUIRES` to help detect leaks.
    ```bash
    valgrind --leak-check=full perl Makefile.PL && make && make test
    ```

## 5. Porting Tests from Python

-   Review the test cases in the Python implementation (`python/google/protobuf/internal/*_test.py`) and port relevant scenarios to Perl `.t` files. This will help ensure feature parity and catch edge cases already coverd in the Python version.

## 6. C-Level Coroutine Safety Testing (`t/c/integration/*_coro.c`)

-   To test the re-entrancy and safety of C functions when used in a concurrent manner, we use the `libcoro` C library (vendored in `perl/libcoro/`).
-   Tests are created in `t/c/integration/` (e.g., `030_protobuf_coro.c`).
-   These C tests *do not* use the Perl `Coro` module. Instead, they use `libcoro` to create and manage C-level coroutines.
-   A dummy Perl interpreter context is generally faked using macros, as the primary goal is to stress the C code's interaction with shared resources like the object cache, not the Perl-C API calls themselves.
-   The `Makefile.PL` adds `libcoro/coro.c` to the `extra_src` for these specific test targets in the `$c_test_config` hash.
-   This allows early detection of C-level race conditions or state corruption issues.

## 7. Perl-Level Coroutine Safety Testing (`xt/author/coro-safe.t`)

-   This test uses the actual Perl `Coro` module.
-   It calls the *Perl API* of the `Protobuf::*` modules from within `async` blocks.
-   This test verifies the safety of the XS glue code when used in a real Perl Coro environment.