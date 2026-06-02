# Critical Perl Coding Reminders for AI

1.  **Regex `/x` Modifier**: Use the `/x` modifier for regular expressions (`m//`, `s///`, `qr//`) where it enhances readability, especially for complex patterns. Remember:
    *   **ESCAPE WHITESPACE**: Literal spaces must be escaped (e.g., `\s`, `\ `).
    *   **ESCAPE HASHES**: Literal `#` characters must be escaped (e.g., `\#`).
    *   **Example**: `s/my\s+key\#1/replacement/x`
    *   Simple regexes like `m/ERROR/i` do not strictly require `/x`.

2.  **`use strict; use warnings;`**: ALWAYS include at the top of all `.pm` and `.pl` files.

3.  **No `use lib`**: NEVER add `use lib` to any file.

4.  **Explicit `return`**: All subroutines MUST end with an explicit `return` statement.

5.  **`eval` Return Value**: ALWAYS check the return value of an `eval { ... }` block, not just `$@`, to determine success. The code inside the `eval` should return true on success.

6.  **`replace` Tool vs. `write_file`**: When modifying files, if the change spans multiple lines or involves complex logic, it is often more reliable to use `read_file`, modify the content in memory, and then use `write_file` to overwrite the file. The `replace` tool requires an exact match for `old_string`, which can be brittle for multi-line changes.

7.  **Mocking**: Follow the strategies outlined in `@cjproc/docs/MOCKING_GUIDELINES.md`.

8.  **`Test::MockModule` Scoping**: ALWAYS scope `Test::MockModule` objects lexically within subtest blocks (`{ ... }`) to prevent mock bleeding between tests.

9.  **Capture Output in Tests**: Use `Capture::Tiny` to capture and assert on any STDOUT or STDERR output from code under test in `lib/`.

10. **Clean TAP Output**: Tests should ONLY produce TAP. Any other output to STDOUT or STDERR is a test failure. Use `Capture::Tiny` to suppress expected output from the code being tested. diagnostic messages in tests should only be printed on failure, e.g., `ok(...) or diag(...)`.

11. **Debugging Output**: Prefer `Data::Dumper` or `Data::Printer` for temporary debugging output within `lib/` files. Remember to remove such statements before considering work complete.
