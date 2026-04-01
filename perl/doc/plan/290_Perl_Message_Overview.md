# Milestone 290: Perl Layer - Protobuf::Message (Overview)

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

## Goal
Implement the core `Protobuf::Message` interface, which dynamically creates accessors and mutators for all fields based on their UPB definitions. Because of the complexity of this task, it has been broken down into several sub-milestones (291-299) to ensure methodical progress and comprehensive testing.

## Sub-Milestones
*   [x] **291**: Message Class Generation & Moo Setup
*   [x] **292**: Scalar Field Accessors (Getters)
*   [x] **293**: Scalar Field Mutators (Setters, Clearers, Has)
*   [x] **294**: Sub-Message Field Accessors & Mutators
*   [x] **295**: Enum Field Handling
*   [x] **296**: Repeated Field Accessors (Scalars)
*   [x] **297**: Repeated Field Accessors (Messages)
*   [x] **298**: Map Field Accessors
*   [x] **299**: Serialization, Deserialization, and Unknown Fields
