# Architecture Overview

This directory contains documentation about the internal architecture of the Perl Protobuf module.

-   [Core Concepts](core/)
    -   [UPB Interfacing](core/01-upb-interfacing.md)
    -   [Memory Management](core/02-memory-management.md)
    -   [Object Caching](core/03-object-caching.md)
    -   [Perl Object Structure](core/04-perl-object-structure.md)
    -   [Type Conversion](core/05-type-conversion.md)
    -   [Error Propagation](core/06-error-propagation.md)
    -   [XS File Organization](core/07-xs-file-organization.md)
-   [API Design](api/)
    -   [DescriptorPool Management](api/01-descriptor-pool.md)
    -   [Descriptor API](api/02-descriptor.md)
    -   [Message Creation](api/03-message-creation.md)
    -   [Message Field Accessors](api/04-message-accessors.md)
    -   [Descriptor Containers](api/05-descriptor-containers.md)
-   [Build System](build/)
-   [Testing Strategy](testing/)
