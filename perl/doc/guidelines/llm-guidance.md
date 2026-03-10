# LLM Assistant Guidance for CJProc Project

This document outlines key guidelines and reminders for the LLM assistant working on this project.

1.  **Follow Conventions:** Strictly adhere to all rules outlined in `conventions.md` and the style guide in `share/prompts/how-to-write-perl-good.md`.

2.  **Thorough Testing:** After ANY code modification (new features, bug fixes, refactoring):
    *   **Always work from the `cjproc` directory for test runs.**
    *   Build the distribution: `perl Makefile.PL && make > /dev/null 2>&1`
    *   Run specific unit tests using `prove -bv t/NN-test_file.t`.
    *   Run any relevant acceptance tests (`t/acceptance/*.t`).
    *   Run the critic test (`t/01_critic.t`).
    *   **Crucially, run the *entire* test suite (`make test`) to catch any unintended side effects or regressions before considering a task complete.**
    *   Do not claim a task is finished until all tests pass without any noise or warnings, unless the warnings are known and accepted (like IPC::Run deprecation).

2.  **Test File Naming:** New test files in the `t/` directory should be named with a two-digit prefix followed by a hyphen (e.g., `t/11-my_module.t`) to ensure consistent ordering when tests are run with globs like `t/*.t`.

3.  **Clarity and Conciseness:** Keep responses clear and to the point, as befits a CLI environment.

4.  **Proactive Problem Solving:** Anticipate potential issues and address them. For example, if adding a dependency, remember to update `Makefile.PL` without being prompted.

5.  **Mocking:** Follow the mocking strategies outlined in `conventions.md`, preferring to mock runner module methods when testing code that uses them.

6.  **`use lib`:** NEVER add `use lib` to any project file.

7.  **Output Redirection:** Silence noisy build commands like `make` by default (e.g., `make > /dev/null 2>&1`).

8.  **Perl::Critic:** Strive to write code that complies with default `Perl::Critic` policies. While `## no critic` annotations are sometimes used as a temporary measure for known issues (like overly complex subroutines needing refactoring), these should be minimized. Plan to address these violations after critical deadlines.

9.  **Regex `/x` Modifier:** Use the `/x` modifier for complex regular expressions to improve readability. It is not mandatory for simple regexes. Remember to escape literal spaces (e.g., `\s`, `\ `) and pound signs (`\#`) within `/x` regexes.

10. **Regex Debugging:** If you encounter persistent `Perl::Critic` errors related to `RegularExpressions::RequireExtendedFormatting` or other regex issues, especially involving escaping or complex patterns, after 1-2 attempts to fix it, please highlight the problematic regex and the error and ask for human assistance. Your ability to self-correct complex regex nuances is limited.

11. **`Test::MockModule` Scoping:** Ensure `Test::MockModule` objects are lexically scoped within subtests (using `{ ... }`) to prevent mocks from leaking between test cases. The mock object should be created *inside* the subtest block.

12. **Cache Testing:** For modules that cache data to the file system, design them to accept a `cache_dir` in the constructor. In tests, use `File::Temp qw(tempdir)` to create a temporary cache directory and pass it to the constructor to ensure test isolation.

13. **Centralize Repeated Logic:** Identify and refactor code blocks that are duplicated across multiple scripts or modules into a single, reusable module or function. This improves maintainability and reduces the chance of inconsistencies.

14. **Defensive Programming with External Data:** Be extra cautious when processing data from external APIs or files. Always validate the structure and type of data before using it. Expect the unexpected, such as `undef` values or incorrect types within arrays or hashes. Use `defined` and `ref` checks liberally.

15. **String Quoting in Hash Keys:** Avoid double quotes for hash keys if they contain variable-like syntax (e.g., "$owner/$repo"). This can lead to unwanted interpolation. Use single quotes or concatenate strings to form the key.

16. **`replace` Tool Usage:** The `replace` tool requires an exact match for `old_string`. For multi-line changes or when unsure about exact whitespace, it's often more reliable to use `read_file`, modify the content in memory, and then use `write_file`.

17. **Import What You Use:** Ensure all necessary modules, including standard ones like `Carp`, are explicitly imported with `use`.

18. **Newline Characters in Strings:** When generating Perl code:
    *   To include a literal newline character, use `\n` ONLY within double-quoted strings (`"...\n..."`), `qq{}` blocks, backticks (`` `...\n...` ``), or `qx{}` blocks.
    *   Inside single-quoted strings (`'...'`) or `q{}` blocks, `\n` is treated as a literal backslash followed by the character 'n', NOT as a newline.
    *   Do *not* use `\n` as a bareword; it's invalid syntax.

19. **Script Execution:** Assume all scripts in `cjproc/bin/` are installed in the user's PATH and can be invoked directly by name (e.g., `promote_exemplary_evidence.pl`) without specifying the full path, unless otherwise instructed. Directory paths for options should be relative to the user's current working directory.

20. **UPB Includes:** Do not include `upb/upb.h`. This file does not exist. Core UPB headers like `upb/def.h` or `upb/message.h` are found through the include paths, usually pointing to the root of the `upb` directory.

21. **Weak References in XS:** When creating weak references in C/XS, use the function `sv_rvweaken(SV *const sv)`. Do not use the hallucinated `sv_weaken(SV *const sv)`. The Perl level function is `Scalar::Util::weaken()`.

22. **Scalar::Util:** The `Scalar::Util` module does not have a corresponding C header file like `<Scalar/Util.h>`. XS code should include the standard Perl headers (`EXTERN.h`, `perl.h`, `XSUB.h`) which provide the necessary functions. To call functions like `weaken`, you need to load the `Scalar::Util` module in Perl and call it through the Perl API if needed, or use the C-level equivalents like `sv_rvweaken` directly.