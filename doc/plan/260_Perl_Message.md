# Milestone 26: Perl Layer - Protobuf::Message (Base Class)

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/04-message-base.t`.
*   [x] Tests in `t/04-message-base.t` cover message creation, get/set, and serialization/parsing.
*   [x] Implement `Protobuf::Message` base class.
*   [x] Consolidate all XS code into `perl/Protobuf.xs` to ensure shared state (pool, obj cache) across all modules.
*   [x] Update Perl modules to use the central `Protobuf` module for XS loading.
*   [x] Implement `_xs_new_from_class` to allow `GeneratedClass->new()` to work.
*   [x] Fix dot-to-double-colon conversion when blessing message objects.
*   [x] Perl tests pass for base message functionality.
*   [x] Review and update documentation.
