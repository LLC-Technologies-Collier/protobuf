# XS File Organization

To enhance maintainability and align with the structure of the Python UPB extension, the C code for the Perl XS layer is organized into subdirectories within `perl/xs/`.

## Directory Structure

Each major component of the Protobuf implementation has its own subdirectory:

-   `perl/xs/convert/`: Type conversion routines.
-   `perl/xs/descriptor/`: Descriptor wrappers and logic.
-   `perl/xs/descriptor_containers/`: Generic containers for descriptor collections.
-   `perl/xs/descriptor_pool/`: DescriptorPool implementation.
-   `perl/xs/extension_dict/`: ExtensionDict implementation.
-   `perl/xs/map/`: Map field implementation.
-   `perl/xs/message/`: Message object implementation.
-   `perl/xs/protobuf/`: Core module utilities, Arena wrapper, Object Cache.
-   `perl/xs/repeated/`: Repeated field implementation.
-   `perl/xs/unknown_fields/`: UnknownFieldSet implementation.

Top-level files like `perl/xs/protobuf.h` may still exist for widely shared declarations.

## Rationale

-   **Modularity:** Keeps code for different components separate and easier to manage.
-   **Alignment:** Makes it easier to reference and compare with the Python UPB C source code.
-   **Scalability:** Allows for breaking down large C files into smaller, more focused units within each component directory. For example, `perl/xs/descriptor/` contains `message.c`, `enum.c`, etc.
