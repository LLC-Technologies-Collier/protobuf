# Testing and Debugging CJProc Reflectors - Guidance

This document outlines lessons learned from debugging the unit tests for `CJProc::BugReflector` and other reflector modules, particularly after schema and AI interaction changes.

## Key Challenges & Solutions:

1.  **JSON Schema Syntax Errors:**
    *   **Problem:** Multiple `replace` operations on the schema files led to subtle syntax errors, primarily missing commas between major blocks (like after `level_alignment` before `effort_alignment`) or stray characters (like `\`). This caused `load_json_file` to fail within the modules and tests.
    *   **Solution:** Carefully validate the JSON syntax after any modifications. Using `jq . <filename>` is a quick way to check. For complex changes, sometimes it's safer to reconstruct sections rather than using many small `replace` calls.
    *   **LLM Guidance:** After using the `replace` tool on JSON files, always suggest a validation step using `jq . <filename> > /dev/null`. If there are errors, read the file and carefully correct the syntax, paying close attention to commas and brackets.

2.  **API Schema Compatibility (`nullable`):
    *   **Problem:** Using `type: ["string", "null"]` for optional fields in the JSON schema, while valid JSON Schema, caused 400 Bad Request errors from the Google AI API. The API's schema parser is stricter.
    *   **Solution:** As per the Gemini API documentation on Structured Output, optional string fields should be defined with `"type": "string"` and `"nullable": true`.
    *   **LLM Guidance:** When defining schemas for the Google AI API, use `"type": "string", "nullable": true` for fields that can be strings or null. Avoid using array types for `type` like `["string", "null"]`.

3.  **Mocking `generate_content` Return Values:
    *   **Problem:** The `CJProc::AI->generate_content` method was refactored to return `($reflection_obj, $result_hash)` instead of `($reflection_obj, $status_string)`.
    *   **Solution:** Unit tests mocking `generate_content` needed to be updated to return the new structure. Successful mocks should return `( { ... }, { status => 'ok' } )`. Failure mocks should return `( undef, { status => 'error', error_message => '...' } )`.
    *   **LLM Guidance:** When mocking a method, ensure the mock's return signature and structure exactly match what the method under test actually returns. If the mocked method's return structure changes, all tests mocking it *must* be updated.

4.  **Identifying Which Test File to Fix:
    *   **Problem:** Test failures like `Can't use string ("SUCCESS") as a HASH ref` clearly point to a mock issue, but the error message shows the line in the *module* file (e.g., `CJProc::BugReflector.pm`), not the *test* file (e.g., `t/21-bug_reflector.t`).
    *   **Solution:** Recognize that the error occurs where the module *uses* the mocked function's return value. The fix is to adjust the mock definition within the corresponding `.t` file.
    *   **LLM Guidance:** When a test fails due to type mismatches or unexpected values related to a mocked function, inspect the `.t` test file for the mock setup of the function mentioned in the call stack. Update the mock to return the expected data structure and values.

5.  **Perl Critic and Style in Tests:
    *   **Problem:** While not causing test *failures*, `Perl::Critic` might flag issues in test files.
    *   **Solution:** Apply the same coding standards from `conventions.md` to test files as well.
    *   **LLM Guidance:** Follow all project coding conventions even in test files.

## General Debugging Strategy for Reflector Tests:

1.  **Validate Schemas:** Always confirm schema syntax first.
2.  **Check Mock Returns:** Ensure `generate_content` mocks in tests match the module's expectations.
3.  **Isolate Failing Tests:** Use `prove -bv <test_file.t>` to focus on one test at a time.
4.  **Read Error Messages Carefully:** Pinpoint the module and line number, but remember the fix is often in the test file's mock setup.
5.  Inspect Module Code:

6.  **Mocking and Cache Isolation:**
    *   **Problem:** Tests interfering with each other due to shared cache state or difficult-to-mock cache paths.
    *   **Solution:** **Strongly prefer Dependency Injection** for cache directories. Design modules to accept a `cache_dir` in their constructor. Tests can then easily provide a unique `File::Temp` directory using `tempdir( CLEANUP => 1 )`.
    *   **Alternative:** If modifying the module to accept `cache_dir` is not feasible, and it uses hardcoded paths based on `File::HomeDir`, you must mock `File::HomeDir->my_home` in a `BEGIN` block *before* the module under test is loaded.
    *   **Testing File Caching:** When testing modules that write to a cache directory, and you are using Dependency Injection to provide a temporary directory, avoid mocking the file system read/write operations (e.g., `read_file_content`, `write_file_content`, `open`, `print`). The test should confirm that cache files are actually created, read, and used as expected within the temporary directory.

7.  **Mocking with `Test::MockModule` and Subtests:**
    *   **Problem:** Mocks from one subtest affecting others.
    *   **Solution:** `Test::MockModule` objects MUST be lexically scoped within each subtest block (`{ ... }`). This ensures mocks are uninstalled when the block is exited, preventing test contamination.
    *   **LLM Guidance:** Always create `Test::MockModule` instances inside a subtest's block. Use separate `Test::MockModule` instances for different packages being mocked.

8.  **Capturing Output:**
    *   **Problem:** Code under test printing to STDOUT or STDERR (e.g., via `print`, `warn`, `carp`, or from mocked dependencies) can cause test failures if not handled.
    *   **Solution:** ALWAYS use `Capture::Tiny` to wrap calls to code that are expected to produce any output to STDOUT or STDERR. Assert on the contents of the captured `$stdout` and `$stderr`.
    *   **LLM Guidance:** If a test fails due to unexpected output, wrap the call to the method under test with `capture { ... }` and add assertions for `$stdout` and `$stderr`.

9.  **Mocking Imported Util Functions:** When a module (`ModuleA`) uses functions imported from another module (`ModuleB` via `use ModuleB qw(some_func)`), to mock how `some_func` behaves *within* `ModuleA`, you must mock the function in `ModuleA`'s namespace:
    ```perl
    # In t/module_a.t
    use ModuleA;
    use Test::MockModule;
    # ...
    my $module_a_mock = Test::MockModule->new('ModuleA');
    $module_a_mock->mock( some_func => sub { /* mock behavior */ } );
    ```

10. **Clearing `Test::MockObject` History:** When reusing a `Test::MockObject` instance across different phases within a single subtest (e.g., testing a cache miss then a cache hit), use `$mock_object->clear()` to reset the call history and any method mocks before setting up new mock behaviors for the next phase.

By following these guidelines, debugging and maintaining the reflector tests should be more straightforward.