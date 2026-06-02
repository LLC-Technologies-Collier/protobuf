# CJProc Coding and Project Conventions

This document outlines the conventions used within the CJProc project.

1.  **Module Namespace:** All modules reside under the `CJProc` namespace.
2.  **Directory Structure:**
    *   `bin/`: Executable scripts.
    *   `lib/`: Perl modules (`.pm`).
    *   `share/`: Data files, templates, and other assets used by the modules/scripts.
    *   `t/`: Test files.
    *   `tmp/`: Temporary files, not part of the distribution.
3.  **Asset Distribution:**
    *   Non-code assets (e.g., prompts, schemas, SQL queries) used by the modules or scripts should be placed within the `share/` directory.
    *   Use `File::ShareDir` to locate these assets within the code. Specifically, use `dist_file('CJProc', 'path/within/share/...')` to get the path to an asset.
    *   Ensure `File::ShareDir::Install` is used in `Makefile.PL` to handle the installation of the `share/` directory contents.
4.  **Configuration Files:**
    *   Scripts in `bin/` generally expect input files (e.g., `okrs.json`, `role-profiles.json`) to be present in the *current working directory* from which the script is executed.

4.  **Scripts in `bin/`:**
    *   All scripts in the `bin/` directory that are intended to be executable MUST start with a shebang line, typically `#!/usr/bin/env perl`.
    *   This is essential because these scripts are listed in `EXE_FILES` in `Makefile.PL` and will be installed in a bin directory in the PERL5LIB path.

5.  **Tandem Assets in `share/`:**
    *   Many modules and scripts rely on a set of files in `share/` which work together. Typically, this includes:
        *   A **prompt file** in `share/prompts/` (usually a `.md` or `.txt` file) containing instructions for the LLM.
        *   A **JSON schema** in `share/schemas/` (a `.json` file) defining the expected output structure from the LLM. The schema filename should generally match the prompt filename.
        *   Optionally, an **SQL file** in `share/sql/` if data needs to be fetched from F1.
    *   Modules should use `dist_file` to load these assets. When interacting with `CJProc::AI`, the schema (with the top-level `$schema` key removed) should be passed in the `generationConfig` to enforce the output structure.

6.  **Perl Style:**
    *   Use `strict` and `warnings`.
    *   Follow general good Perl practices. (More specific style guidelines can be added here as needed).
    *   **Avoid `use lib`:** Do not use `use lib` to modify @INC. Modules should be properly installed (e.g., via `cpanm .`) so they are found in the standard @INC paths, or the PERL5LIB environment variable should be set externally.
    *   **Temporary Debugging:** `use Data::Dumper` in conjunction with `warn` is permissible within `lib/` modules *only* for short-term debugging and must be removed before code is considered final. `Test::More::diag` and `Data::Dumper` can be used more liberally within `t/` files.
    *   **Attribute Defaults:** Avoid using functions with side effects (e.g., file I/O, network calls, `dist_file`) within `has` attribute defaults. This can make testing difficult and lead to unexpected behavior during class loading. Instead, initialize such values within the `BUILD` method or lazily in the methods that first require them, or accept them as constructor arguments.
    *   **Regular Expressions:** Use the `/x` modifier for complex regular expressions to improve readability by allowing whitespace and comments. Remember to escape literal whitespace (e.g., `\s`, `\ `) and `#` (`\#`) within `/x` blocks. It is not mandatory for very simple regexes where it doesn't aid clarity.
        ```perl
        # GOOD:
        if ($text =~ m/ \s+ stuff \s+ # Match with padding /x) { ... }

        # Optional /x for simple cases:
        if ($text =~ m/ERROR/i) { ... }
        if ($text =~ m/ERROR/ix) { ... } # Also fine
        ```
    *   **Alternative Delimiters:** To avoid escaping forward slashes `/` within regexes, use alternative delimiters. Common choices include `m{...}`, `s{...}{...}`, `qr{...}`. You can also use matched pairs like `<...>`, `(...)`, or `[...]` if those characters don't appear within the regex content itself.
        ```perl
        # GOOD: Alternative delimiters
        if ($text =~ m{/some/path}x) { ... }
        $text =~ s{/old/path}{/new/path}gx;
        my $pattern = qr<https?://[^/]+>;
        ```
    *   **Subroutine Endings:** All subroutines must end with an explicit `return` statement, even if it's just `return;`.
    *   **Conditional Logic:** Avoid `unless` with negative conditions (e.g., `ne`, `!~`, `!`). Use `if` instead for better readability.
    *   **`eval` Blocks:** Always check the return value of an `eval { ... }` block to determine success, in addition to checking `$@` for an error message.
    *   **External Commands:** Do not use backticks (`` ` ``). Use `IPC::Run::run` or `Capture::Tiny::capture`.
    *   **Variable Scope:** Avoid re-declaring `my` variables with the same name in overlapping scopes where it might be confusing.
    *   **File Paths:** Use `File::Spec::Functions` for cross-platform path manipulation (e.g., `catfile`, `catdir`). Use `File::Basename qw(basename dirname)` for extracting parts of a path. Do not rely on string manipulation for paths.
    *   **Method Naming:** Avoid overly generic method names that might clash with functions exported by commonly used modules (e.g., avoid `run` if using `IPC::Run`). Prefix internal or wrapper methods with an underscore (e.g., `_run_command`).
    *   **String Quoting:**
        *   Use single quotes (`''`) for strings that do not require variable interpolation or special escape sequences. This is generally preferred for literal strings.
        *   Use double quotes (`""`) when variable interpolation (e.g., `"$var"`) or escape sequences like `\n`, `\t` are needed.
        *   Be cautious with `@` in double-quoted strings, as it can trigger array interpolation. If you need a literal `@` in a double-quoted string, escape it (`\@`), but it's often cleaner to use single quotes if no other interpolation is needed (e.g., `'user@example.com'`).
        *   `q{}` and `qq{}` can be used as alternatives to single and double quotes, respectively, especially useful if the string itself contains many single or double quotes.
    *   **Substitution Regexes:** Be aware that the match part of a substitution `s/MATCH/REPLACEMENT/` behaves like a double-quoted string. Sigils like `@` and `$` will be interpolated. If you are using a placeholder like `@@LDAP@@`, you MUST escape the `@` symbols (e.g., `s/\@\@LDAP\@\@/$ldap/g`) to prevent Perl from trying to interpolate an array named `@LDAP`.

6.  **Logging:**
    *   Use `Log::Log4perl` for logging.
    *   Initialize in scripts using `Log::Log4perl->easy_init()`.
    *   To control the level from a variable, use a hash to map string names to level constants:
        ```perl
        use Log::Log4perl qw(:easy);
        use Log::Log4perl::Level;
        my %levels = (
            TRACE => $TRACE, DEBUG => $DEBUG, INFO  => $INFO,
            WARN  => $WARN,  ERROR => $ERROR, FATAL => $FATAL,
        );
        my $level_const = $levels{uc $log_level_str} || $WARN;
        Log::Log4perl->easy_init($level_const);
        ```
    *   Use methods like `$log->info()`, `$log->debug()`, etc. Do not use `infof`, `debugf`, etc., which are not standard Log::Log4perl methods.

7.  **Makefile.PL:**

    *   Executable scripts should be listed in the `EXE_FILES` array.

    *   Dependencies are managed in `PREREQ_PM` and `TEST_REQUIRES`.

    *   The `share/` directory is installed via `File::ShareDir::Install`.


7.  **Installation:**

    *   Before installing, ensure the distribution builds and tests cleanly by running `perl Makefile.PL && make && make test` from the root of the `cjproc` directory.
    *   Modules should be installed from the distribution directory using `cpanm .` (which uses the standard Perl library paths).
    *   **DO NOT** use the `--local-lib` option with `cpanm` for general development or deployment, as this can create inconsistencies with the system's Perl environment. Use it only if you have a specific reason to install to a non-standard location.
    *   **NEVER EVER** use the force flag (`-f`) with `cpanm`. If tests fail, the tests *must* be fixed. Forcing installation masks underlying problems, leads to an unstable environment, and is a serious breach of development protocol. If a forced installation seems absolutely necessary, it requires justification and approval from a project admin.


8.  **MANIFEST:**
    *   The `MANIFEST` file is managed by `ExtUtils::MakeMaker`.
    *   A `MANIFEST.SKIP` file is used within the distribution's root directory to exclude build artifacts, temporary files, and version control directories from the distribution.
    *   Regenerate the manifest using `perl Makefile.PL && make manifest` from within the distribution's root directory after adding or removing files.
8.  **Testing:**
    *   Tests are located in the `t/` directory.
    *   Tests are run via `make test` from within the distribution's root directory.
    *   **During development, it is often more efficient to run only the test file(s) relevant to the changes being made using `prove -bv t/your_test_file.t`.**
    *   **After any non-trivial change, always run the full `make test` suite to ensure no regressions before considering the work complete.**
    *   Remember that `subtest` blocks require the form `subtest 'name' => sub { ... };`.
    *   **Strict TAP Output:** The output of `make test` must be clean TAP only. Any other output to STDOUT or STDERR indicates a failure.
    *   **Test File (`*.t`) Output:**
        *   Test scripts should not produce any output to STDOUT or STDERR *except* for TAP and conditional diagnostic messages.
        *   **Use `diag` Conditionally:** It is highly encouraged to use `diag()` *only* upon assertion failure to provide context, for example: `ok($success, "Test description") or diag Dumper($data);`.
    *   **Capturing Output from Library Code (`lib/`):**
        *   If code in `lib/` is designed to print to STDOUT or warn/carp to STDERR, any test executing that code *MUST* wrap the call with `Capture::Tiny`.
        *   Assertions *MUST* then be made on the captured `$stdout` and `$stderr` to verify the exact expected output. Uncaptured output from `lib/` code will cause the test run to fail.
        ```perl
        use Capture::Tiny 'capture';
        my ($stdout, $stderr, $result) = capture {
            $module->method_that_prints();
        };
        is($stdout, "Expected output\n", "STDOUT is correct");
        is($stderr, "", "SDBERR is empty");
        ```
    *   **Test Data Precision:** Be meticulous with test data, especially in here-docs. Extra whitespace within lines can lead to unexpected parsing behavior, for example, with CSVs.

9.  **JSON Schema Files:**
    *   Schema files should be placed in `share/schemas/`.
    *   Schema files should be named to match the prompt or data structure they define, using the suffix `.json`. For example, the schema for `share/prompts/my-prompt.md` should be named `share/schemas/my-prompt.json`.

10. **Mocking in Tests:**


    *   **a) Mocking External Dependencies (e.g., CPAN modules like `IPC::Run`, `File::ShareDir`, or CLI tools):**
        *   **STRONGLY Preferred Method: Wrap and Mock Internal Methods:** Instead of directly calling functions imported from EXTERNAL modules (e.g., `IPC::Run::run`, `File::ShareDir::dist_file`) or executing external commands, create a small, private wrapper method or a dedicated wrapper module (e.g., `CJProc::Fetcher::F1Runner`) within the `CJProc` namespace that makes the call.
            ```perl
            # In YourModule.pm
            # For external functions
            sub _get_dist_file { my $self = shift; return File::ShareDir::dist_file(@_); }

            # For external commands (using a dedicated runner module)
            has 'f1_runner' => (is => 'ro', default => sub { CJProc::Fetcher::F1Runner->new() });
            sub _run_f1_query { my $self = shift; return $self->f1_runner->run_query(@_); }
            ```
        *   In your tests, use `Test::MockModule` to mock these *internal wrapper methods* or inject mock objects for the wrapper modules. This isolates the test from the external dependency's complexities and makes the mock less brittle.
        *   This approach is generally more robust than mocking the imported function in the caller's namespace (e.g., `YourModule::run`), which can be fragile.
        *   **Avoid `BEGIN` blocks for mocking external dependencies:** This is usually not necessary if the wrapper pattern is used.

    *   **b) Mocking Internal Dependencies (between `CJProc` modules):**
        *   **Dependency Injection:** Design constructors to accept instances of other `CJProc` modules they depend on. Tests can then inject mock objects.
        *   **Mocking Imported Functions:** If `ModuleA` uses `use ModuleB qw(function_b);`, to mock `function_b`'s behavior *within* `ModuleA`, mock the function in `ModuleA`'s namespace: `Test::MockModule->new('ModuleA')->mock('function_b', sub { ... });`.
        *   **Mocking Object Methods:** Use `Test::MockModule` to mock methods on instances of other `CJProc` modules.

    *   **c) Mocking Object Method Calls:**
        *   When `ModuleA` instantiates `ModuleB` (e.g., `my $b = ModuleB->new();`) and then calls methods on the instance (e.g., `$b->do_something()`), the best way to mock this in `t/module_a.t` is:
            1.  Mock `ModuleB->new` to return a `Test::MockObject` instance.
            2.  Mock the method `do_something` on the `Test::MockObject` instance.

            ```perl
            # In t/module_a.t
            use Test::More;
            use Test::MockModule;
            use Test::MockObject;
            use ModuleA;

            # Mock ModuleB->new to return our mock object
            my $mock_b_instance = Test::MockObject->new();
            my $module_b_mock = Test::MockModule->new('ModuleB');
            $module_b_mock->mock(new => sub { $mock_b_instance });

            # Now, mock the method call on the instance
            $mock_b_instance->mock(do_something => sub {
                # Your mock implementation for do_something
                return "mocked result";
            });

            my $a = ModuleA->new();
            is($a->call_b_do_something(), "mocked result", "ModuleB->do_something was mocked");

            # Important to unmock if other tests use ModuleB differently
            # $module_b_mock->unmock('new');
            ```
        *   This ensures that `ModuleA`'s code, which expects to call methods on an object, is satisfied, and you control the behavior of those method calls within the test.

    *   **d) Mocking before Module Load:** (Rarely needed for internal mocks)
        *   If a mock must be in place *before* the module under test is compiled (e.g., for code in `BEGIN` blocks), this requires careful setup, often involving a `BEGIN` block in the test *before* the `use ModuleUnderTest;` line.

11. **Refactoring:**

12. **Acceptance Tests:**
    *   For critical user journeys, typically initiated by scripts in `bin/`, acceptance tests should be created in the `t/` directory (e.g., `t/acceptance/`).
    *   These tests should execute the `bin/` scripts as a user would, using `IPC::Run` or similar, to capture STDOUT, STDERR, and exit codes.
    *   **Mocking Limitations:** Be aware that mocks defined within the test script (e.g., using `Test::MockModule`) **DO NOT** affect the separate process launched by `IPC::Run`. To test interactions with dependencies, you must either:
        *   Rely on checking side effects (e.g., file creation, database state).
        *   Design the script to accept configuration or environment variables that tests can manipulate to point to test doubles or fakes.
        *   Analyze log files generated by the script.
    *   **Environment:** Ensure `PERL5LIB` is correctly set to include `blib/lib` and `blib/arch` when running scripts via `IPC::Run` to make project modules available:
        ```perl
        my $blib_lib  = abs_path(catfile( $Bin, '..', 'lib' ));
        my $blib_arch = abs_path(catfile( $Bin, '..', 'arch' ));
        local $ENV{PERL5LIB} = join ':', $blib_lib, $blib_arch, ($ENV{PERL5LIB} || '');
        run \@cmd, ...;
        ```
    *   They should verify the script's major outcomes, such as the creation of expected output files, correct exit status, and key information in the output.
    *   These tests will often involve setting up temporary input files.
    *   Example: `t/acceptance/analyze_okr_script.t` would run `bin/analyze-okr.pl sample.csv` and check that `sample.json` is created with the expected structure.
    *   **Module Size:** Strive to keep modules under 200 lines, and ideally under 100 lines. Larger modules should be candidates for refactoring.
    *   **Separation of Concerns:** Break down modules based on distinct responsibilities.
    *   **Helper/Utility Modules:** Extract reusable logic into well-named helper modules, often within a submodule namespace (e.g., `CJProc::MyModule::Helper`).
    *   **Moo Roles:** Consider using Moo::Roles to share common method implementations between classes where inheritance doesn't fit.
    *   **Testability:** Refactoring should often make the code easier to test by isolating units of functionality.

13. **Commit Messages:**
    *   Follow the Conventional Commits specification: `type(scope): description`.
    *   **Types:** `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`.
    *   **Scope:** Optional, can be the module or area affected (e.g., `AI`, `Parser`, `testing`).
    *   **Description:** Concise summary of the change in the present tense.
    *   Example: `refactor(AI): Extract retry logic to RetryManager`
    *   Example: `fix(Parser): Handle CSVs with Windows line endings`
    *   Example: `docs: Clarify output capturing in test conventions`

14. **AI Code Generation:**
    *   When generating or modifying code, the AI assistant (i.e., YOU, Gemini) MUST proactively apply ALL conventions and style guidelines outlined in this document and in `@cjproc/share/prompts/how-to-write-perl-good.md`.
    *   The goal is to produce code that passes `t/01_critic.t` and other tests on the *first attempt*.
    *   **Recent Perl::Critic Focus Areas:**
        *   `RegularExpressions::RequireExtendedFormatting`: Use `/x` for complex regexes.
        *   `RegularExpressions::ProhibitUnescapedMetacharacters`: Escape literal spaces `\s` or `\ ` in `/x` regexes.
        *   `Subroutines::RequireFinalReturn`: Ensure every sub ends with `return`.
        *   `ErrorHandling::RequireCheckingReturnValueOfEval`: Check the return of `eval { ... }`, not just `$@`.
        *   `ControlStructures::ProhibitUnlessConditions`: Avoid `unless` with negative logic.
        *   `InputOutput::ProhibitHeredocTerminatorQuote`: Quote HERE-doc terminators (e.g., `<<'END_SQL';`).
        *   `InputOutput::RequireBriefOpen`: Close filehandles as soon as possible.
        *   `ValuesAndExpressions::ProhibitMagicNumbers`: Avoid unexplained numbers.
        *   `Naming::ProhibitImplicitNumericWrite`: Ensure numeric conversions are explicit if needed (e.g., `$num + 0`).
