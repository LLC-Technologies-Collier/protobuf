# Perl XS Development Cycle for Protobuf Module

This document outlines the typical iterative development and testing cycle used for the `Protobuf` Perl module, which involves both Perl `.pm` files and C/XS `.xs` files.

1.  **Modify Code:** Make changes to the `.pm` files (e.g., `lib/Protobuf/Message.pm`), `.xs` files (e.g., `lib/Protobuf/Message.xs`), or the `typemap`.

2.  **Regenerate Makefile (if needed):** If you add new `.xs` files or change dependencies in `Makefile.PL`, you MUST re-run `perl Makefile.PL`. This is generally safe to do even if not strictly necessary.

3.  **Build:** Compile the changes. This updates the compiled components in the `blib/` directory.

    ```bash
    make
    ```
    *(Output is redirected to temporary files to reduce noise).*

    # To rebuild the common library used by C tests:
    make libprotobuf_common.a

4.  **Run Specific Tests:** Run the test file(s) most relevant to the changes made. The `-b` flag tells `prove` to use the modules in `blib/`. The `-v` flag provides verbose output.

    ```bash
    prove -bv t/05_message.t
    ```

5.  **Debug & Iterate:** If tests fail:
    *   Analyze the error messages and TAP output.
    *   Inspect code in `.pm` or `.xs` files.
    *   Add temporary `warn` statements or use `Data::Dumper` in Perl, or `fprintf(stderr, ...)` in XS for debugging.
    *   Re-run `make` and the specific `prove` command after fixes.
    *   For C tests in `t/c/`, you may need to force a re-link after changes:
        ```bash
        rm -f t/c/your_test && make t/c/your_test
        ```

6.  **Memory Check (if needed):** If a crash occurs (like a SEGV) or memory issues are suspected, run the relevant test under `Test::Valgrind` by ensuring `Test::Valgrind` is loaded in the `.t` file.

    ```bash
    prove -bv t/04_descriptor_pool.t
    ```
    Then analyze the `.err` output file specified in the `run_shell_command` for Valgrind reports.

7.  **Full Test Suite:** Once the specific tests pass, run the entire test suite to check for regressions:

    ```bash
    make test
    ```

8.  **Clean Build (Optional):** If encountering strange issues, a clean build can help:

    ```bash
    make realclean
    perl Makefile.PL
    make
    make test
    ```

This cycle ensures that changes to both Perl and XS code are compiled and tested effectively. Frequent rebuilding with `make` is crucial after any code modification before running tests.