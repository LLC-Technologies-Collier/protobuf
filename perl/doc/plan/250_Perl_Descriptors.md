# Milestone 25: Perl Layer - Descriptors (File, Message, Enum, Field)

[TOC]

*   [x] REFRESH: Review all documents in @perl/doc/guidelines/**

*   [x] Create test file `t/03-descriptors.t`.
*   [x] Tests in `t/03-descriptors.t` cover File, Message, Enum, and Field descriptors.
*   [x] Implement `Protobuf::Descriptor::File` module and XS.
*   [x] Implement `Protobuf::Descriptor::Message` module and XS.
*   [x] Implement `Protobuf::Descriptor::Enum` module and XS.
*   [x] Implement `Protobuf::Descriptor::Field` module and XS.
*   [x] Rename XS classes from `Protobuf::*Descriptor` to `Protobuf::Descriptor::*` for consistency.
*   [x] Fix `FindFieldByName` issue using manual iteration in the XS wrapper (working around UPB lookup issues).
*   [x] Perl tests pass for all descriptors.
*   [x] Review and update perl/doc/architecture/api/02-descriptors.md (Note: file might be named differently, updated 01-descriptor-pool.md already).
