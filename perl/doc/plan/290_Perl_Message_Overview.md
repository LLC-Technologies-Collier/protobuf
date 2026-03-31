# Milestone 290: Perl Layer - Protobuf::Message (Overview)

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**

## Goal
Implement the core `Protobuf::Message` interface, which dynamically creates accessors and mutators for all fields based on their UPB definitions. Because of the complexity of this task, it has been broken down into several sub-milestones (291-299) to ensure methodical progress and comprehensive testing.

## Sub-Milestones
*   [ ] **291**: Message Class Generation & Moo Setup
*   [ ] **292**: Scalar Field Accessors (Getters)
*   [ ] **293**: Scalar Field Mutators (Setters, Clearers, Has)
*   [ ] **294**: Sub-Message Field Accessors & Mutators
*   [ ] **295**: Enum Field Handling
*   [ ] **296**: Repeated Field Accessors (Scalars)
*   [ ] **297**: Repeated Field Accessors (Messages)
*   [ ] **298**: Map Field Accessors
*   [ ] **299**: Serialization, Deserialization, and Unknown Fields
