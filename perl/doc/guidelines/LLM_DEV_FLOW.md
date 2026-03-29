# LLM Development Flow Guidelines for This Project

1.  **Iterate Small, Test Often:**
    *   Break down tasks into the smallest possible functional units (e.g., one C wrapper function, its test, and config update).
    *   **After each unit:**
        *   Re-run `perl Makefile.PL > /dev/null 2>&1 && make -j$(nproc)`. Check for compilation errors.
        *   Run the specific new test: `prove -bv t/c/descriptor/new_test.c` (adjust path).
        *   Run all C tests: `make test_c`.
    *   This catches errors early and makes debugging much simpler.

2.  **C/XS Compilation Issues:**
    *   **Include Order:** Always include system headers (`<sys/types.h>`, `<stdlib.h>`, etc.) *before* any project or Perl headers (`upb/...`, `EXTERN.h`, `perl.h`).
    *   **`-D_GNU_SOURCE`:** Ensure `-D_GNU_SOURCE` is added to the `CFLAGS` in `Makefile.PL` for `.c` file compilations.
    *   **Check `upb` Headers:** When wrapping `upb` functions, double-check the exact function names and types in the `upb` header files (e.g., `upb/reflection/def.h`, `upb/base/descriptor_constants.h`).

3.  **Test-Driven C Wrappers:**
    *   Write the `.h` declaration.
    *   Write the `.c` implementation.
    *   Immediately write a test case in the `t/c/descriptor/<name>.c` file.
    *   Update `t/c/c_test_config.json`.
    *   Test.

4.  **JSON File Integrity:**
    *   After modifying JSON files like `c_test_config.json` with `replace`, always validate the syntax:
        ```bash
        jq . < perl/t/c/c_test_config.json > /dev/null
        ```
    *   If there's an error, use `read_file` to get the content and carefully correct the JSON structure.

5.  **Be Precise with `replace`:**
    *   The `old_string` must be an *exact* match. Use `read_file` to confirm the exact text if a `replace` fails.
    *   For multi-line or complex changes, prefer `read_file`, modify, then `write_file`.

6.  **Error Message Debugging:**
    *   C compiler errors: Check include order, type names, and function signatures against upb headers.
    *   Test failures (`make test_c`): Use `fprintf(stderr, ...)` in C test files to debug.
    *   `Makefile.PL` errors: Likely JSON syntax in `c_test_config.json`.

7.  **Stick to the Plan:** Follow the component order in the milestone plan document.
