# How to Write Good Perl Code (for LLMs)

This guide is for AI assistants writing or modifying Perl code, especially within the `CJProc` project.

1.  **Strict and Warnings:** ALWAYS start every `.pm` and `.pl` file with:
    ```perl
    use strict;
    use warnings;
    ```

2.  **Package Declarations:** Every `.pm` file must start with a `package` declaration matching its file path (e.g., `lib/CJProc/MyModule.pm` starts with `package CJProc::MyModule;`). Test files (`.t`) do not strictly need a `package` declaration if they are not acting as modules.

3.  **Module Endings:** Every `.pm` file must end with `1;`.

4.  **No `use lib`:** NEVER add `use lib` to any file. Dependencies should be handled by `Makefile.PL` and the build process.

5.  **Lexical Variables:** Declare all variables with `my`. Avoid package variables unless absolutely necessary and clearly justified.

6.  **Error Handling:**
    *   Use `Carp`'s `croak` for fatal errors in modules, and `carp` for warnings. In scripts, `die` is acceptable for fatal errors.
    *   **Checking `eval`:** When using `eval { ... }`, you MUST check the return value of the `eval` itself to determine success, not just the value of `$@`. The code inside the `eval` should return a true value on success.
        ```perl
        # GOOD:
        my $result = eval {
            # code that returns true on success
            do_something_that_might_die();
            return 1;
        };
        if (!$result) {
            # Handle error, $EVAL_ERROR / $@ has the death message
            carp "Failed to do something: $@";
            return;
        }
        # Proceed on success

        # BAD: Only checking $@
        # eval {
        #     do_something_that_might_die();
        # };
        # if ($@) { ... } # Insufficient
        ```

7.  **File Operations:**
    *   Always check the return value of `open`.
    *   Use lexical filehandles: `open my $fh, ...`.
    *   Close filehandles explicitly: `close $fh;`.
    *   Use 3-argument `open` with explicit mode: `open my $fh, '<:encoding(UTF-8)', $filename or croak "..."`.
    *   When using here-documents with `<<~` for test data, ensure there's no unintended leading whitespace *within* each line's content, only for the block's indentation.
    *   **Prefer `Path::Tiny`:** For file and directory operations, use `Path::Tiny` over built-in functions or `File::Spec` where possible. It provides a more modern, object-oriented, and often safer interface.
        ```perl
        use Path::Tiny;
        my $content = path($filename)->slurp_utf8;
        path($output_file)->spew_utf8($new_content);
        path($dir)->mkpath();
        ```
    *   **Path Manipulation:** Use `File::Spec::Functions` for joining path components  (e.g., `catfile`, `catdir`). Use `File::Basename qw(basename dirname)` for extracting parts of a path.

8.  **Regular Expressions:**
    *   **Use the `/x` modifier** for complex regular expressions to improve readability by allowing whitespace and comments. Remember to escape literal whitespace (e.g., `\s`, `\ `) and `#` (`\#`) within `/x` blocks.
    *   The `/x` modifier is not required for simple, single-line regexes where it does not significantly improve clarity.
    *   Use `qr//` to precompile regexes if they are used multiple times.
    *   Escape other meta-characters properly as usual.
        ```perl
        # GOOD Example with /x:
        my $pattern = qr/ \A \s+ foo \s+ # Match foo at the start with spaces
                         bar \s+ \# \s* baz # followed by bar and baz
                       /x;
        if ($text =~ $pattern) { ... }

        # Simple regexes don't require /x:
        if ($text =~ m/ERROR/i) { ... }
        ```
    *   **Alternative Delimiters:** To avoid escaping forward slashes `/` within regexes, use alternative delimiters. Common choices include `m{...}`, `s{...}{...}`, `qr{...}`. You can also use matched pairs like `<...>`, `(...)`, or `[...]` if those characters don't appear within the regex content itself.
        ```perl
        # GOOD: Alternative delimiters
        if ($text =~ m{/some/path}) { ... }
        $text =~ s{/old/path}{/new/path}g;
        my $pattern = qr<https?://[^/]+>;
        ```

9.  **Subroutine Arguments:** Unpack `@_` at the beginning of the subroutine: `my ($self, $arg1, $arg2) = @_;`.

10. **Subroutine Endings:** ALWAYS end every subroutine with an explicit `return` statement. Even if the subroutine doesn't naturally return a value, end it with `return;` to satisfy `Perl::Critic`'s `Subroutines::RequireFinalReturn` policy.
    ```perl
    sub my_sub {
        my ($self, $arg) = @_; 
        # Do stuff
        return $result; # Good
    }

    sub another_sub {
        my $self = shift;
        # Do side effects
        return; # Good - explicit return
    }

    # BAD - Missing final return
    # sub bad_sub { my $self = shift; $self->do_something(); }
    ```

11. **Moo:** When using Moo, include `use Moo;` at the top. Remember that attributes are defined with `has`.
    *   **Setters:** Use the accessor method to set or change an attribute's value (e.g., `$obj->attribute('new_value')`).
    *   **Reference Attributes:** If an attribute holds a reference (e.g., HashRef, ArrayRef) and you fetch it, you are working with the *actual* reference. Modifications to the contents of the fetched reference will persist in the object's attribute without needing to call the setter again with the same reference.
        ```perl
        # In Level.pm
        # has contribution => (is => 'rw', default => sub { { description => '', details => [] } });

        # In parser
        my $contrib = $level_obj->contribution; # Get the HashRef
        $contrib->{description} = "New"; # Modify in place
        push @{ $contrib->{details} }, "New Detail";
        # $level_obj->contribution($contrib); # This is NOT needed
        ```
    *   Updates to nested structures within the reference are immediately reflected in the attribute.

11. **Testing:**
    *   Write tests for all new functionality.
    *   Use `Test::More`.
    *   Use `subtest` for grouping related tests.
    *   **Strict TAP Output:** The `make test` process must *only* produce TAP output. Any other output on STDOUT or STDERR means a test has failed.
    *   **Capturing Expected Output:** If your code in `lib/` deliberately prints to STDOUT or uses `warn`/`carp` for STDERR, your tests *MUST* use `Capture::Tiny` to capture this output. Assert on the captured `$stdout` and `$stderr`.
    *   **Conditional Diagnostics:** Use `diag Dumper($data)` *only* after a failed assertion to provide context: `ok($condition) or diag Dumper($data);`.


12. **Mocking:** Follow the mocking conventions outlined in `@cjproc/conventions.md`. Prefer wrapping external calls and dependency injection for internal modules.

13. **Perl::Critic:** Write code that passes `perlcritic` based on the project's configuration. Pay attention to:
    *   Unused variables.
    *   Subroutine argument counts (don't pass unused arguments).
    *   Proper error reporting.
    *   **Subroutines::RequireFinalReturn:** ALWAYS end every subroutine with an explicit `return` statement.
    *   **RegularExpressions::RequireExtendedFormatting:** Use `/x` for complex regexes.
    *   **ControlStructures::ProhibitUnlessConditions:** Avoid `unless` with negative conditions.
    *   **ErrorHandling::RequireCheckingReturnValueOfEval:** ALWAYS check the return value of `eval { ... }` blocks.
    *   **BuiltinFunctions::ProhibitBooleanGrep:** Avoid `grep` in boolean context. Use `List::Util::any` or similar if appropriate.
    *   **InputOutput::ProhibitBackticks:** Avoid using backticks. Use `IPC::Run::run` or `Capture::Tiny::capture` for capturing command output.
    *   **Variables::ProhibitReusedNames:** Do not reuse variable names in the same lexical scope if it can cause confusion.

14. **Conditional Logic:** Avoid negative conditions within `unless` blocks. Use `if` with a positive equivalent or an `if` with the negated condition for clarity.
    ```perl
    # GOOD:
    if ($a == $b) { ... }
    if ($a != $b) { ... }
    unless ($a == $b) { ... } # Same as if ($a != $b)

    # BAD: Avoid double negatives
    # unless ($a != $b) { ... } # Harder to read - use if ($a == $b)
    # unless ($text !~ /pattern/x) { ... } # Use if ($text =~ /pattern/x)
    ```

15. **Clarity:** Prioritize clear and maintainable code. Add comments only to explain *why* something is done, not *what* is being done.

16. **Makefile.PL Defines:** Use the `DEFINE` parameter in `WriteMakefile` to pass C preprocessor flags (like `-DPERL_NO_PPPORT`) to the compiler for all XS and C files in the module. This is more reliable than `#define` directives within individual source files for global settings.

17. Method Naming: Avoid method names that conflict with functions exported by other commonly used modules (e.g., avoid `run` if you are also using `IPC::Run` in the same scope). Prefix internal or wrapper methods with an underscore (e.g., `_do_something`).

17. **NO `no critic` - ABSOLUTELY NOT:** Do not use `## no critic` annotations to suppress warnings. EVER. Address the underlying code issue to satisfy Perl::Critic. The policies are in place for a reason: to promote robust, readable, and maintainable code. Bypassing them is a sign of weakness and leads to technical debt. Fix the code, don't hide the smell.

17. **Temporary Debugging:** Using `Data::Dumper` with `warn` within module files (`lib/`) is acceptable *only* for temporary debugging purposes. These debugging statements *must* be removed before considering the code complete. Test code in `t/` can use `Test::More::diag` with `Data::Dumper` more freely, especially when combined with `or diag Dumper(...)` on failing assertions.

18. **String Quoting:**
    *   **Single Quotes:** Use single quotes (`''`) or `q{}` (with any matched pair of non-alphanumeric delimiters) for strings that do *not* require variable interpolation or most escape sequences (except `\'` and `\\`). This is preferred for literal strings.
    *   **Double Quotes:** Use double quotes (`""`) or `qq{}` (with any matched pair of non-alphanumeric delimiters) when you need to interpolate variables (e.g., `"$var"`, `qq{$var}`) or use escape sequences (e.g., `"\n"`, `qq{\n}`).
    *   **Caution with `@`:** The `@` symbol is used to sigil arrays (e.g., `@my_array`). In double-quoted strings, Perl will try to interpolate arrays. If you need a literal `@` symbol within a double-quoted string (e.g., in an email address), escape it with a backslash: `"user\@example.com"`. However, it is often much clearer and safer to use single quotes for strings containing literal `@` symbols: `'user@example.com'`, as single quotes do not interpolate arrays.
    *   Be mindful of context, especially when writing code strings for tools like `replace`.
    *   **Substitution Regexes:** Be aware that the match part of a substitution `s/MATCH/REPLACEMENT/` behaves like a double-quoted string. Sigils like `@` and `$` will be interpolated. If you are using a placeholder like `@@LDAP@@`, you MUST escape the `@` symbols (e.g., `s/\@\@LDAP\@\@/$ldap/g`) to prevent Perl from trying to interpolate an array named `@LDAP`.

19. **Handle Perl::Critic Violations:**
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
    *   **InputOutput::RequireBriefOpen:** This policy means you should not keep filehandles open longer than necessary. When you open a file for reading, read its entire contents into memory (e.g., an array of lines, or slurping the whole file) immediately, then close the filehandle. Process the data from memory. This avoids tying up resources and potential issues with leaving files open.
        ```perl
        # GOOD: Read all at once
        open my $fh, '<:encoding(UTF-8)', $filename or croak "Cannot open $filename: $!";
        my @lines = <$fh>;
        close $fh;
        # Now process @lines

        # ALSO GOOD: Slurp
        use Path::Tiny;
        my $content = path($filename)->slurp_utf8;
        # Now process $content
        ```
    *   **Consult Policy PDFs:** Perl::Critic violations often reference specific policies (e.g., `Modules::RequireUseStrict`). These policies are based on Damian Conway's *Perl Best Practices*. If you encounter a violation and are unable to resolve it after two attempts, you should consult the relevant documentation. The documentation for each policy is available in a PDF file named after the policy within the `pbp/policy_docs/` directory. The filename format is `Policy_Name.pdf` (e.g., `pbp/policy_docs/InputOutput_RequireBriefOpen.pdf`). Remember to replace `::` with `_` in the policy name to form the filename. Reviewing this PDF should provide the necessary context and examples to address the violation.
    *   Refer to the specific policy documentation if the violation message isn't clear.

20. **Running Tests:**
    *   To run the entire test suite, change into the distribution directory (`cjproc`) and run:
        ```bash
        cd cjproc
        perl Makefile.PL && make && make test
        ```
    *   To run a single test file for debugging, use `prove` with the `-b` (blib) and `-v` (verbose) flags from the `cjproc` directory:
        ```bash
        cd cjproc
        # Ensure modules are built, or run this after the full make test
        # perl Makefile.PL && make
        prove -bv t/some_test.t
        ```
    *   Always run tests from the root of the distribution directory (`cjproc`).
    *   Treat any non-TAP output as a failure. Use `Capture::Tiny` in tests to handle expected output.

21. **Refactoring Large Modules:**
    *   **TDD Approach:** When refactoring components into new modules or classes:
        1.  **Write Tests First:** Create the test file for the *new* module (e.g., `t/0X-new-module.t`). Write tests that define the expected behavior and interface of the new module. Include positive and negative test cases.
        2.  **Run Tests (Expect Failure):** Run the new test file. The tests should fail because the module or its methods are not yet implemented.
        3.  **Implement an Interface:** Create the skeleton of the new module (e.g., `lib/CJProc/NewModule.pm`) with the necessary `package` declaration, `use Moo;`, `use strict;`, `use warnings;`, and empty `sub` definitions for the methods tested.
        4.  **Implement Logic:** Flesh out the methods in the new module with the refactored logic.
        5.  **Run Tests (Expect Success):** Run the new test file. Iterate on the implementation until all tests for the new module pass.
        6.  **Integrate:** Update the original module to *use* the new module. This might involve removing the old code and calling the new module's methods.
        7.  **Run All Tests:** Run the *entire* test suite (`make test`) to ensure the integration hasn't broken anything else.
    *   **Identify Responsibilities:** When a module exceeds 150-200 lines, analyze if it's handling too many distinct responsibilities.
    *   **Single Responsibility Principle:** Aim for modules that do one thing well.
    *   **Helper Modules:** Consider moving blocks of logic into helper modules. For example, complex data transformation, I/O operations for a specific format, or interactions with a particular external service.
    *   **Orchestration:** If a module's primary role becomes orchestrating calls to several other helper modules, this is often a good separation of concerns. The main module remains lean, and the complexity is encapsulated within the helpers.
    *   **Dependency Injection:** Design helper modules to be injected into the main module's constructor for better testability and looser coupling.
    *   **Example:** Instead of having a single `DataProcessor.pm` that reads, transforms, validates, and writes, you might have `DataReader.pm`, `DataTransformer.pm`, `DataValidator.pm`, and `DataWriter.pm`, with `DataProcessor.pm` orchestrating the flow between them.

22. **Text::CSV_XS:**
    *   When not using `auto_diag`, you MUST check the return values of parsing functions (e.g., `parse`, `getline_hr`).
    *   CRITICAL: After a parsing loop, always check `$csv->error_diag()` to catch any errors, as `$csv->eof()` might be true even if the last record failed to parse completely.
        ```perl
        while ( my $row = $csv->getline_hr($fh) ) {
            # process $row
        }
        if ( $csv->error_diag() ) {
            my $error = $csv->error_diag();
            croak "CSV parsing error: $error";
        }
        ```
    *   Consider `allow_loose_quotes => 1` if you need to handle CSVs with less strict quoting.

23. **Installation:**
    *   Always build and test before installing: `perl Makefile.PL && make && make test`.
    *   Install using `cpanm .` from the `cjproc` root directory.
    *   **ABSOLUTELY DO NOT USE `cpanm -f` (force).** This is a cardinal sin. If tests fail, fix the tests. Forcing an installation is unacceptable and hides real issues.
    *   Avoid `cpanm --local-lib` unless explicitly required for a specific, isolated reason, as it can cause environment divergence.

24. **Logging with Log::Log4perl:**
    *   When using `Log::Log4perl->easy_init()`, to set the level dynamically from a string variable (e.g., from command line options), you must map the string to the level constants provided by `Log::Log4perl::Level`.
        ```perl
        use Log::Log4perl qw(:easy);
        use Log::Log4perl::Level; # Required for $DEBUG, $INFO, etc.

        my $log_level_str = 'INFO'; # From input
        my %levels = (
            TRACE => $TRACE, DEBUG => $DEBUG, INFO  => $INFO,
            WARN  => $WARN,  ERROR => $ERROR, FATAL => $FATAL,
        );
        my $level_const = $levels{uc $log_level_str} || $WARN;
        Log::Log4perl->easy_init($level_const);
        ```
    *   Use the standard logger methods: `$log->debug("message")`, `$log->info("message")`, etc. Avoid `debugf` or `infof` as they are not part of core `Log::Log4perl`.

25. **Acceptance Testing Scripts via IPC::Run:**
    *   Mocks in the test file (e.g., `Test::MockModule`) do not affect the child process running the script.
    *   Set `PERL5LIB` to include `blib` paths to make modules available to the script:
        ```perl
        my $blib_lib  = abs_path(catfile( $Bin, '..', 'lib' ));
        my $blib_arch = abs_path(catfile( $Bin, '..', 'arch' ));
        local $ENV{PERL5LIB} = join ':', $blib_lib, $blib_arch, ($ENV{PERL5LIB} || '');
        run \@cmd, ...;
        ```
    *   Test script behavior by checking side effects (e.g., files created) or by having the script log key information that the test can capture or read.

## IMPORTANT: Adherence and Efficiency

**It is CRITICAL to follow all guidelines laid out in this document and `@cjproc/conventions.md`. Failure to do so will result in test failures. This will require additional cycles of correction and re-testing, which is inefficient. Strive to produce compliant code from the outset.**

**Proactive Compliance:** As an AI, your primary goal is to generate code that is correct and compliant *from the start*. Before outputting any code, mentally review it against these guidelines and the `@cjproc/conventions.md` file. Pre-emptively fix potential issues.

By following these guidelines, you will produce more robust, maintainable, and conventional Perl code.
