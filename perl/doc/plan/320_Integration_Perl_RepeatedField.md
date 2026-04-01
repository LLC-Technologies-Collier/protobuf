# Milestone 32: Integration - Perl RepeatedField

[TOC]

*   [x] Create test file `t/integration/320_repeated_field.t`
*   [ ] TODO: Implement Direct Array-to-Array Deep Copy.
*   [ ] TODO: Implement Shared-Arena Array Slicing.
*   [ ] TODO: Verify Cross-Interpreter Container Migration Stress.
*   [x] Tests cover interactions between Protobuf::RepeatedField and Protobuf::Message.
*   [x] Integration tests pass.
*   [x] Create and implement `xt/coro/320_repeated_field.t` to test Coro safety.
*   [ ] TODO: Stress concurrent repeated field mutation (100 Coros).
*   [ ] TODO: Verify Coro re-entrancy during array subset slicing.
*   [x] Create and implement `xt/mojo/320_repeated_field.t` to test Mojo::IOLoop concurrency safety.
*   [ ] Create and implement `xt/anyevent/320_repeated_field.t` to test AnyEvent concurrency safety. (TODO)
*   [x] Review and update perl/doc/architecture/** documents.