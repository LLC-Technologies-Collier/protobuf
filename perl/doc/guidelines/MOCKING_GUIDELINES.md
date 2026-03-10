# CJProc Mocking Guidelines

This document consolidates best practices for mocking in the CJProc project.

1.  **Lexical Scoping of `Test::MockModule`**: ALWAYS create `Test::MockModule` objects inside the lexical block (`{ ... }`) of the subtest where the mock is needed. This ensures mocks are uninstalled when the block is exited, preventing test contamination between subtests.

2.  **Mocking External Dependencies (e.g., CPAN modules like `IPC::Run`, `File::ShareDir`)**:
    *   **STRONGLY Preferred: Wrap and Mock Internal Methods**: Instead of directly calling functions imported from EXTERNAL modules, create a small, private wrapper method within your module (e.g., `_run_ipc`, `_get_dist_file`). In tests, mock these internal wrapper methods using `Test::MockModule`. This is more robust. See `@cjproc/docs/conventions.md#Mocking in Tests`.
    *   **Direct Mocking of Imported Functions**: If wrapping is not feasible, you must mock the function in the *caller's* namespace. For example, if `MyModule` uses `use IPC::Run qw(run);`, you mock `MyModule::run`. This mock must be applied *after* `use MyModule;`.
    *   **BEGIN Blocks for Compile-Time Needs**: If an external module function is used at compile time (e.g., in an attribute default), any mock must be set up in a `BEGIN` block *before* the module under test is loaded. This is often error-prone; avoid side effects in defaults.

3.  **Mocking Internal Dependencies (between `CJProc` modules)**:
    *   **Dependency Injection (Preferred)**: Design modules to accept instances of other `CJProc` modules they depend on in the constructor. Tests can then easily inject `Test::MockObject` instances or specialized test doubles.
    *   **Mocking Methods**: Use `Test::MockModule` to mock methods on other `CJProc` module classes or instances.

4.  **Mocking Object Method Calls (`$b = ModuleB->new(); $b->do_something()` in `ModuleA`)**:
    *   In `t/module_a.t`, mock `ModuleB->new` to return a `Test::MockObject` instance.
    *   Mock the `do_something` method on the `Test::MockObject` instance.
    *   See `@cjproc/docs/conventions.md#Mocking Object Method Calls` for an example.

5.  **`IPC::Run`**: Directly mocking the `run` function imported from `IPC::Run` is fragile. ALWAYS use the "Wrap and Mock Internal Methods" pattern for `IPC::Run` calls.

6.  **`File::ShareDir`**: Avoid using `dist_file` in attribute defaults. Call it at runtime, preferably within a wrapped internal method that can be easily mocked.

7.  **Cache Testing**: Use Dependency Injection to pass the `cache_dir` to the constructor. In tests, provide a `File::Temp` directory.

8.  **Clearing `Test::MockObject` History**: Use `$mock_object->clear()` between test phases within the same subtest if you are reusing the mock object instance but need to reset expectations or mock behaviors.

By following these guidelines, mocking should be more predictable and tests more robust.
