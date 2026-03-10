# Perl XS Development Learnings (Protobuf Edition)

[TOC]

This document captures key lessons learned during the development of the Perl XS module for Protocol Buffers, interfacing with the `upb` library.

## 1. Memory Management & Object Lifecycles

*   **Dangling Pointers are Easy to Create:** When Perl objects wrap C pointers, the lifecycle of the C data MUST be tied to the Perl object. If the C data is freed while Perl objects still hold pointers to it, SEGV is inevitable.
*   **Inter-Object References:** If a Perl object (e.g., `Protobuf::Descriptor`) holds a pointer to data owned by *another* Perl object's C counterpart (e.g., `upb_MessageDef` owned by `Protobuf::DescriptorPool`'s `upb_DefPool`), the descriptor object **MUST** hold a strong Perl reference to the pool object. Otherwise, the pool can be garbage collected, freeing the C memory and causing the descriptor to point to garbage.
    *   **Solution:** Store a reference to the owner object within the dependent object (e.g., `has '_pool' => (is => 'ro', required => 1);` in `Protobuf::Descriptor`).
*   **Arena Management:** When `upb` parses data (like `FileDescriptorSet`), the arena used for parsing must live as long as the definitions are needed by the `upb_DefPool`. Attaching the arena to the `Protobuf::DescriptorPool` object lifecycle is crucial.

## 2. XS Build & Compilation

*   **`PERL_NO_PPPORT`:** To reliably prevent `ppport.h` from being included and causing type/macro conflicts, add `-DPERL_NO_PPPORT` to the `DEFINE` option in `Makefile.PL`. Simply defining it in C files is not sufficient as it may not be active early enough during the inclusion of Perl's core headers.
*   **`make realclean` is Your Friend:** When .xs files change, especially function signatures or dependencies, `make clean` might not be enough. `make realclean` forces a more thorough cleanup, including `blib/` and the Makefile itself, ensuring a fresh build. Always re-run `perl Makefile.PL` after `make realclean`.
*   **Timestamps Matter:** `make` relies on timestamps. If a `.c` file generated from `.xs` isn't older than the `.xs` file, it might not recompile. `touch` can be used to update the `.xs` timestamp to force recompilation.
*   **XSUB Naming Collisions:** Be wary of naming conflicts between Perl subroutines defined in `.pm` files and XSUBs exposed from `.xs` files. If they have the same fully qualified name, the XSUB will typically overwrite the Perl sub at runtime.  
    *   **Solution:** Name internal XSUBs with a leading underscore or use a different naming convention to disambiguate (e.g., `_internal_xs_version`).
*   **Typemaps & XSUB Arguments:** When an XSUB signature in the `.xs` file includes typemapped C types (e.g., `upb_DefPool *`), you must pass a corresponding Perl variable of the type expected by the `INPUT` section of the typemap from the Perl caller. If the typemap expects to extract a C pointer from a Perl object, you must pass that Perl object.
*   **`dTHX` for C Functions:** Pure C functions not using the full XSUB macro boilerplate (like `MODULE = ...`) still need `dTHX;` at the start if they interact with the Perl API (e.g., `croak`, `newSViv`, `call_method`).
*   **`hv_delete_ent` Refcounting:** The `hv_delete_ent` function decrements the refcount of the key SV passed to it. Do not decref the lookup key SV again after this call.
*   **Object Cache Keys:** Use stringified pointer addresses as hash keys in the object cache for reliable lookups.
*   **Consolidate Core XS Helpers:** Common C helper functions for the XS layer (e.g., Arena wrappers, string utilities) should be placed in `xs/protobuf.c` and declared in `xs/protobuf.h` to ensure they are part of `libprotobuf_common.a` and available to C tests.

## 3. C Test Harnesses (`t/c/`)

*   **Embedding Perl for Tests:**
    *   Initialize with `PERL_SYS_INIT(&argc, &argv);`
    *   Call `perl_alloc()` then `perl_construct()`.
    *   `perl_parse()` is necessary. Critically, pass it `"-e", "0"` to prevent it from blocking on STDIN. Pass a pointer to an empty `xs_init` function as the second argument.
    *   **Do not call `PERL_SYS_TERM()`**, as this can cause mutex-related panics during shutdown in this context. `perl_destruct()` and `perl_free()` are sufficient.
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
*   **Makefile.PL for C Tests:** C tests in `t/c/` must be added to the `$c_test_config` in `MY::postamble` within `perl/Makefile.PL` to receive the correct compiler and linker flags, including those from `ExtUtils::Embed::ccopts()` and `ExtUtils::Embed::ldopts()`.
*   **Rebuilding C Tests:** `make` may not automatically recompile C tests when only `Makefile.PL` changes. To force a re-link, you may need to `rm -f t/c/your_test_binary` before running `make t/c/your_test_binary`.

## 4. Debugging XS

*   **`-g` Flag:** Ensure `-g` is added to `CCFLAGS` in `Makefile.PL` to include debug symbols in the compiled objects and shared libraries.
*   **GDB:** Running `gdb --args perl -Iblib/lib -Iblib/arch t/test_file.t` is essential for debugging SEGV's. Set breakpoints in your C/XS functions.
*   **Valgrind:** `Test::Valgrind` can catch memory errors, but output can be noisy with Perl & Moo internals. Focus on `InvalidRead`, `InvalidWrite`, and `DefinitelyLost` errors originating from your module's code.
*   **Diagnostic Prints:** `fprintf(stderr, ...)` in C/XS can be helpful, but ensure they flush (e.g., add `\n`).

## 4.1. Debugging by Instrumenting UPB

When a segmentation fault or inexplicable behavior occurs, and it's suspected to originate from the interaction with the `upb` library itself, a powerful debugging technique is to temporarily modify the `upb` source files (under `@upb/`) by adding `fprintf(stderr, ...)` statements. Since we cannot permanently modify the `upb` code, this is a transient debugging aid.

**Process:**

1.  **Identify Potential UPB Functions:** Based on the crash context, pinpoint the `upb` functions that are likely involved (e.g., `upb_FieldDef_MessageSubDef`, functions within `def_pool.c`, etc.).
2.  **Add Debug Prints:** Insert `fprintf(stderr, "[DEBUG] UPB: %s:%d ...\n", __FILE__, __LINE__, ...)` calls within the `upb` source files to trace execution flow and inspect variable values.
3.  **Rebuild UPB:** Since the Perl module's build doesn't recompile `upb` from source, you need to trigger a rebuild of the `upb` components used by the Perl build. The exact command depends on the project setup, but it often involves a Bazel command:
    ```bash
    bazel build //upb/...
    ```
4.  **Rebuild Perl Module:** Clean and rebuild the Perl module to link against the newly instrumented `upb` code:
    ```bash
    cd perl
    make realclean
    perl Makefile.PL
    make -j$(nproc)
    ```
5.  **Run Tests:** Execute the C tests (`make test_c`) or Perl tests (`prove -bv t/some_test.t`) that were causing the issue.
6.  **Analyze Output:** Examine the STDERR output for the `[DEBUG] UPB:` messages to understand the state and flow within `upb` leading up to the crash.
7.  **Clean Up:** Once the issue is found, **it is crucial to revert the changes** made to the `@upb/` source files.

This method provides deep insights into the `upb` library's internal state, which is invaluable for debugging complex crashes related to the C interface.

## 5. Moo and XS

*   **`BUILDARGS`:** To customize constructor argument processing in a Moo class with XS, define a C function (e.g., `Protobuf_Message_BUILDARGS`) and point to it from the `BUILDARGS` sub in your `.pm` file: `sub BUILDARGS { &Protobuf_Message_BUILDARGS }`.
*   **Attribute Initialization:** Lazy builders in Moo are a good way to defer XS calls for object attribute initialization until first use.
*   **`DEMOLISH`:** Be careful in `DEMOLISH` blocks to check for the existence and definedness of hash keys pointing to C pointers before attempting to free them, as the object might not have been fully constructed if `new()` croaked.

## 6. Segfault in t/c/convert/upb_to_sv.c (Dec 2025)

*   **Symptom:** Segfault at the very end of the `make test_c` run, after all test output from `t/c/convert/upb_to_sv` had been printed.
*   **Initial Misdiagnosis:** Believed to be an issue within the `PerlUpb_UpbToSv` function call for message types, as prints before the call worked, but not at the function entry.
*   **Debugging Steps:**
    1.  Extensive `fprintf` in `PerlUpb_UpbToSv` and the test runner.
    2.  Added ASan flags to `Makefile.PL` (`CCFLAGS` and `LDDLFLAGS`). ASan did not initially flag the root cause.
    3.  Instrumented the `set_message_val` function in `t/c/convert/types/message.c`.
    4.  Corrected message name from `test.NestedMessage` to `perltest.NestedMessage`.
    5.  Added more debug prints to the `PerlUpb_WrapMessage` stub.
    6.  Fixed `const` correctness in `PerlUpb_WrapMessage` signature.
    7.  Instrumented the `main` function's teardown sequence in `t/c/convert/upb_to_sv.c`.
*   **Root Cause:** A copy-paste error led to `upb_DefPool_Free(test_pool);` being called twice in `t/c/convert/upb_to_sv.c`'s `main` function. This double free corrupted memory, causing a segfault during the final stages of program exit.
*   **Lesson:** When segfaults occur at the end of a test, pay close attention to cleanup code in the test's `main` function. Double-free errors can manifest late. Also, meticulously check string literals used in lookups.

By keeping these points in mind, developing Perl XS extensions can be made more robust and less error-prone.