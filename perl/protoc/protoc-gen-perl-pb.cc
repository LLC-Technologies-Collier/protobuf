#include "google/protobuf/compiler/plugin.upb.h"
#include "google/protobuf/descriptor.upb.h"
#include "upb/reflection/def.h"
#include "upb/mem/arena.h"
#include "upb/base/status.h"
#include "upb/base/string_view.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <cctype>
#include <iterator>

// Function to capitalize the first letter of a string
std::string capitalize(std::string s) {
    if (!s.empty()) {
        s[0] = std::toupper(s[0]);
    }
    return s;
}

// Function to convert snake_case to CamelCase
std::string to_camel_case(const std::string& s) {
    std::string result = "";
    bool capitalize_next = true;
    for (char c : s) {
        if (c == '_') {
            capitalize_next = true;
        } else if (capitalize_next) {
            result += std::toupper(c);
            capitalize_next = false;
        } else {
            result += c;
        }
    }
    return result;
}

// Function to base64 encode
std::string base64_encode(const std::string& in) {
    std::string out;
    int val = 0, valb = -6;
    const std::string base64_chars =
                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                 "abcdefghijklmnopqrstuvwxyz"
                 "0123456789+/";
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

// Converts package to Perl module name base
std::string get_module_base_name(const std::string& package) {
    std::string module_name = "";
    if (!package.empty()) {
        std::stringstream ss(package);
        std::string segment;
        bool first = true;
        while(std::getline(ss, segment, '.')) {
            if (!first) module_name += "::";
            module_name += capitalize(segment);
            first = false;
        }
    }
    return module_name;
}

std::string get_perl_package_name(const std::string& proto_file_path, const std::string& package) {
    std::string base = get_module_base_name(package);
    
    std::string proto_basename = proto_file_path;
    size_t last_slash = proto_basename.find_last_of('/');
    if (last_slash != std::string::npos) {
        proto_basename = proto_basename.substr(last_slash + 1);
    }
    size_t last_dot = proto_basename.find_last_of('.');
    if (last_dot != std::string::npos) {
        proto_basename = proto_basename.substr(0, last_dot);
    }
    proto_basename = to_camel_case(proto_basename);
    
    if (base.empty()) {
        return proto_basename;
    }
    return base + "::" + proto_basename;
}

// Converts a Perl package name to a module file path
std::string package_name_to_module_path(const std::string& package_name) {
    std::string path = package_name;
    size_t pos = 0;
    while ((pos = path.find("::", pos)) != std::string::npos) {
        path.replace(pos, 2, "/");
        pos += 1;
    }
    return path + ".pm";
}

bool embed_descriptors = false;
bool generate_services = false;
char* out_dir = NULL;

void parse_options(const std::string& options) {
    std::vector<std::string> parts;
    std::stringstream ss(options);
    std::string item;
    while (std::getline(ss, item, ',')) {
        parts.push_back(item);
    }

    for (const auto& part : parts) {
        if (part == "embed_descriptors") {
            embed_descriptors = true;
        } else if (part.rfind("embed_descriptors=", 0) == 0) {
            std::string val = part.substr(18);
            embed_descriptors = (val == "true" || val == "1" || val == "yes");
        } else if (part == "generate_services") {
            generate_services = true;
        } else if (part.rfind("generate_services=", 0) == 0) {
            std::string val = part.substr(18);
            generate_services = (val == "true" || val == "1" || val == "yes");
        } else if (part.rfind("out_dir=", 0) == 0) {
            if (out_dir) free(out_dir);
            out_dir = strdup(part.substr(8).c_str());
        }
    }
}

// Helper to create a upb_StringView from a std::string on a given arena
upb_StringView upb_StringView_FromStringView(const std::string& str, upb_Arena* arena) {
    char* data = (char*)upb_Arena_Malloc(arena, str.length());
    if (!data) return upb_StringView_FromString("");
    memcpy(data, str.data(), str.length());
    return upb_StringView_FromDataAndSize(data, str.length());
}

int main(int argc, char* argv[]) {
    upb_Arena arena;
    upb_Arena_Init(&arena, 0, NULL);

    std::string stdin_content;
    std::cin.seekg(0, std::ios::end);
    size_t length = std::cin.tellg();
    if (length > 0) {
        stdin_content.resize(length);
        std::cin.seekg(0, std::ios::beg);
        std::cin.read(&stdin_content[0], length);
    }

    google_protobuf_compiler_CodeGeneratorRequest* request =
        google_protobuf_compiler_CodeGeneratorRequest_parse(stdin_content.data(), stdin_content.size(), &arena);

    if (!request) {
        std::cerr << "Failed to parse CodeGeneratorRequest from stdin." << std::endl;
        upb_Arena_Free(&arena);
        return 1;
    }

    upb_StringView params = google_protobuf_compiler_CodeGeneratorRequest_parameter(request);
    if (params.size > 0) {
        parse_options(std::string(params.data, params.size));
    }

    upb_DefPool* pool = upb_DefPool_New();
    if (!pool) {
        std::cerr << "Failed to create upb_DefPool." << std::endl;
        upb_Arena_Free(&arena);
        return 1;
    }

    size_t num_proto_files;
    const google_protobuf_FileDescriptorProto* const* proto_files =
        google_protobuf_compiler_CodeGeneratorRequest_proto_file(request, &num_proto_files);

    for (size_t i = 0; i < num_proto_files; ++i) {
        upb_Status status;
        upb_Status_Clear(&status);
        if (!upb_DefPool_AddFile(pool, proto_files[i], &status)) {
            upb_StringView name = google_protobuf_FileDescriptorProto_name(proto_files[i]);
            std::cerr << "Failed to add FileDescriptorProto to pool: "
                      << std::string(name.data, name.size) << " - "
                      << upb_Status_ErrorMessage(&status) << std::endl;
            upb_DefPool_Free(pool);
            upb_Arena_Free(&arena);
            return 1;
        }
    }

    google_protobuf_compiler_CodeGeneratorResponse* response =
        google_protobuf_compiler_CodeGeneratorResponse_new(&arena);

    size_t num_files_to_generate;
    const upb_StringView* files_to_generate =
        google_protobuf_compiler_CodeGeneratorRequest_file_to_generate(request, &num_files_to_generate);

    for (size_t i = 0; i < num_proto_files; ++i) {
        const google_protobuf_FileDescriptorProto* proto_file = proto_files[i];
        upb_StringView proto_file_name_sv = google_protobuf_FileDescriptorProto_name(proto_file);
        std::string proto_file_name(proto_file_name_sv.data, proto_file_name_sv.size);

        bool generate = false;
        for (size_t j = 0; j < num_files_to_generate; ++j) {
            if (files_to_generate[j].size == proto_file_name_sv.size &&
                memcmp(files_to_generate[j].data, proto_file_name_sv.data, proto_file_name_sv.size) == 0) {
                generate = true;
                break;
            }
        }
        if (!generate) {
            continue;
        }

        upb_StringView package_sv = google_protobuf_FileDescriptorProto_package(proto_file);
        std::string package(package_sv.data, package_sv.size);
        std::string package_name = get_perl_package_name(proto_file_name, package);
        std::string module_path = package_name_to_module_path(package_name);

        std::stringstream content_ss;
        content_ss << "package " << package_name << ";" << std::endl;
        content_ss << R"(use strict;)" << std::endl;
        content_ss << R"(use warnings;)" << std::endl;
        content_ss << R"(use Protobuf::Message;)" << std::endl;
        content_ss << R"(use Protobuf::Internal qw(:all);)" << std::endl;

        bool has_any_enums = false;
        size_t enum_count;
        google_protobuf_FileDescriptorProto_enum_type(proto_file, &enum_count);
        if (enum_count > 0) has_any_enums = true;

        size_t message_count;
        const google_protobuf_DescriptorProto* const* messages =
            google_protobuf_FileDescriptorProto_message_type(proto_file, &message_count);
        if (!has_any_enums && message_count > 0) {
            for (size_t j = 0; j < message_count; ++j) {
                size_t nested_enum_count;
                google_protobuf_DescriptorProto_enum_type(messages[j], &nested_enum_count);
                if (nested_enum_count > 0) {
                    has_any_enums = true;
                    break;
                }
            }
        }

        if (has_any_enums) {
            content_ss << R"(use Const::Fast;)" << std::endl;
        }
        if (embed_descriptors) {
            content_ss << R"(use MIME::Base64;)" << std::endl;
        }
        content_ss << std::endl;

        if (embed_descriptors) {
            size_t serialized_fd_size;
            const char* serialized_fd_data = google_protobuf_FileDescriptorProto_serialize(proto_file, &arena, &serialized_fd_size);
            if (!serialized_fd_data) {
                 google_protobuf_compiler_CodeGeneratorResponse_set_error(response, upb_StringView_FromString("Failed to serialize FileDescriptorProto"));
                 // TODO: Proper error handling - free and return
                 return 1;
            }
            std::string serialized_fd(serialized_fd_data, serialized_fd_size);
            std::string b64_descriptor = base64_encode(serialized_fd);

            content_ss << R"(BEGIN {)" << std::endl;
            content_ss << R"(    my $descriptor_b64 = <<'END_DESC';)" << std::endl;
            size_t chunk_size = 64;
            for (size_t k = 0; k < b64_descriptor.length(); k += chunk_size) {
                content_ss << b64_descriptor.substr(k, chunk_size) << std::endl;
            }
            content_ss << R"(END_DESC)" << std::endl;
            content_ss << R"(    Protobuf::DescriptorPool::get_generated_pool()->add_serialized_file()" << std::endl;
?
/, $descriptor_b64))))" << std::endl;
            content_ss << R"(    );)" << std::endl;
            content_ss << R"(})" << std::endl;
        }
        content_ss << std::endl << std::endl;

        // Generate Enums
        if (has_any_enums) {
             google_protobuf_FileDescriptorProto_enum_type(proto_file, &enum_count);
            if (enum_count > 0) {
                const google_protobuf_EnumDescriptorProto* const* enums =
                    google_protobuf_FileDescriptorProto_enum_type(proto_file, &enum_count);
                for (size_t j = 0; j < enum_count; ++j) {
                    const google_protobuf_EnumDescriptorProto* enum_proto = enums[j];
                    upb_StringView enum_name_sv = google_protobuf_EnumDescriptorProto_name(enum_proto);
                    std::string enum_name(enum_name_sv.data, enum_name_sv.size);

                    size_t value_count;
                    const google_protobuf_EnumValueDescriptorProto* const* values =
                        google_protobuf_EnumDescriptorProto_value(enum_proto, &value_count);
                    for (size_t k = 0; k < value_count; ++k) {
                        const google_protobuf_EnumValueDescriptorProto* value_proto = values[k];
                        upb_StringView value_name_sv = google_protobuf_EnumValueDescriptorProto_name(value_proto);
                        std::string value_name(value_name_sv.data, value_name_sv.size);
                        int32_t value_number = google_protobuf_EnumValueDescriptorProto_number(value_proto);
                        content_ss << "const my $" << value_name << " => " << value_number << ";" << std::endl;
                    }
                    content_ss << std::endl;
                }
            }
        }

        // Generate Services
        if (generate_services) {
            size_t service_count;
            const google_protobuf_ServiceDescriptorProto* const* services =
                google_protobuf_FileDescriptorProto_service(proto_file, &service_count);
            if (service_count > 0) {
                content_ss << "# Service definitions would go here." << std::endl << std::endl;
                for (size_t j = 0; j < service_count; ++j) {
                    const google_protobuf_ServiceDescriptorProto* service_proto = services[j];
                    upb_StringView service_name_sv = google_protobuf_ServiceDescriptorProto_name(service_proto);
                    std::string service_name(service_name_sv.data, service_name_sv.size);
                    content_ss << "# Service: " << service_name << std::endl;

                    size_t method_count;
                    const google_protobuf_MethodDescriptorProto* const* methods =
                        google_protobuf_ServiceDescriptorProto_method(service_proto, &method_count);
                    for (size_t k = 0; k < method_count; ++k) {
                        const google_protobuf_MethodDescriptorProto* method_proto = methods[k];
                        upb_StringView method_name_sv = google_protobuf_MethodDescriptorProto_name(method_proto);
                        std::string method_name(method_name_sv.data, method_name_sv.size);
                        content_ss << "#   Method: " << method_name << std::endl;
                    }
                    content_ss << std::endl;
                }
            }
        }

        // Generate Messages
        // size_t message_count; // Already declared above
        // const google_protobuf_DescriptorProto* const* messages = // Already declared above
        if (message_count > 0) {
            content_ss << "# Message definitions" << std::endl << std::endl;
            for (size_t j = 0; j < message_count; ++j) {
                const google_protobuf_DescriptorProto* msg_proto = messages[j];
                upb_StringView msg_name_sv = google_protobuf_DescriptorProto_name(msg_proto);
                std::string msg_name(msg_name_sv.data, msg_name_sv.size);
                std::string full_msg_name = package_name + "::" + msg_name;

                content_ss << "# ============================================================================" << std::endl;
                content_ss << "# Message: " << full_msg_name << std::endl;
                content_ss << "# ============================================================================" << std::endl;

                // TODO: Generate package declaration for the message (e.g., package My::Package::MyMessage;)
                // This is not strictly necessary if all messages are in the same file as the main package,
                // but good practice for clarity if we ever split them.

                // TODO: Collect needed 'use' statements for field types
                std::vector<std::string> use_statements;

                // Fields
                size_t field_count;
                const google_protobuf_FieldDescriptorProto* const* fields =
                    google_protobuf_DescriptorProto_field(msg_proto, &field_count);
                if (field_count > 0) {
                    content_ss << "    # Fields" << std::endl;
                    for (size_t k = 0; k < field_count; ++k) {
                        const google_protobuf_FieldDescriptorProto* field_proto = fields[k];
                        upb_StringView field_name_sv = google_protobuf_FieldDescriptorProto_name(field_proto);
                        std::string field_name(field_name_sv.data, field_name_sv.size);
                        content_ss << "    # Field: " << field_name << std::endl;
\
                        google_protobuf_FieldDescriptorProto_Type type = google_protobuf_FieldDescriptorProto_type(field_proto);
                        if (type == google_protobuf_FieldDescriptorProto_TYPE_MESSAGE || type == google_protobuf_FieldDescriptorProto_TYPE_ENUM) {
                            upb_StringView type_name_sv = google_protobuf_FieldDescriptorProto_type_name(field_proto);
                            std::string type_name(type_name_sv.data, type_name_sv.size);
                            if (!type_name.empty() && type_name[0] == '.') {
                                type_name = type_name.substr(1);
                            }
                            // TODO: Convert this fully qualified type name to a Perl module name
                            // and add to use_statements if it's from a different file.
                            content_ss << "    #   Type Name: " << type_name << std::endl;
                        }
                        // TODO: Generate accessors/mutators (if not dynamic)
                    }
                    content_ss << std::endl;
                }

                // TODO: Output unique 'use' statements collected above
                for (const auto& use_stmt : use_statements) {
                    content_ss << use_stmt << std::endl;
                }
                content_ss << std::endl;


                // Nested Enums
                size_t nested_enum_count;
                const google_protobuf_EnumDescriptorProto* const* nested_enums =
                    google_protobuf_DescriptorProto_enum_type(msg_proto, &nested_enum_count);
                if (nested_enum_count > 0) {
                    content_ss << "    # Nested Enums for " << msg_name << std::endl;
                    for (size_t k = 0; k < nested_enum_count; ++k) {
                        const google_protobuf_EnumDescriptorProto* nested_enum_proto = nested_enums[k];
                        upb_StringView nested_enum_name_sv = google_protobuf_EnumDescriptorProto_name(nested_enum_proto);
                        std::string nested_enum_name(nested_enum_name_sv.data, nested_enum_name_sv.size);
                        content_ss << "    # Enum: " << nested_enum_name << std::endl;

                        size_t nested_value_count;
                        const google_protobuf_EnumValueDescriptorProto* const* nested_values =
                            google_protobuf_EnumDescriptorProto_value(nested_enum_proto, &nested_value_count);
                        for (size_t l = 0; l < nested_value_count; ++l) {
                            const google_protobuf_EnumValueDescriptorProto* nested_value_proto = nested_values[l];
                            upb_StringView nested_value_name_sv = google_protobuf_EnumValueDescriptorProto_name(nested_value_proto);
                            std::string nested_value_name(nested_value_name_sv.data, nested_value_name_sv.size);
                            int32_t nested_value_number = google_protobuf_EnumValueDescriptorProto_number(nested_value_proto);
                            content_ss << "    const my $" << msg_name << "_" << nested_value_name << " => " << nested_value_number << ";" << std::endl;
                        }
                    }
                    content_ss << std::endl;
                }

                // Nested Messages
                size_t nested_message_count;
                const google_protobuf_DescriptorProto* const* nested_messages =
                    google_protobuf_DescriptorProto_nested_type(msg_proto, &nested_message_count);
                if (nested_message_count > 0) {
                    content_ss << "    # Nested Messages" << std::endl;
                    for (size_t k = 0; k < nested_message_count; ++k) {
                        const google_protobuf_DescriptorProto* nested_msg_proto = nested_messages[k];
                        upb_StringView nested_msg_name_sv = google_protobuf_DescriptorProto_name(nested_msg_proto);
                        content_ss << "    # Message: " << std::string(nested_msg_name_sv.data, nested_msg_name_sv.size) << std::endl;
                        // TODO: Recursively generate nested message definitions
                    }
                    content_ss << std::endl;
                }

                // TODO: Generate extension ranges
                size_t extension_range_count;
                const google_protobuf_DescriptorProto_ExtensionRange* const* extension_ranges =
                    google_protobuf_DescriptorProto_extension_range(msg_proto, &extension_range_count);
                if (extension_range_count > 0) {
                    content_ss << "    # Extension Ranges" << std::endl;
                    for (size_t k = 0; k < extension_range_count; ++k) {
                        const google_protobuf_DescriptorProto_ExtensionRange* range = extension_ranges[k];
                        content_ss << "    #   Range: " << google_protobuf_DescriptorProto_ExtensionRange_start(range)
                                   << " to " << google_protobuf_DescriptorProto_ExtensionRange_end(range) - 1 << std::endl;
                        // TODO: Register extension ranges with the runtime
                    }
                    content_ss << std::endl;
                }

                content_ss << std::endl;
            }
            content_ss << std::endl;

            // Class registration
            content_ss << "# Register Messages" << std::endl;
            for (size_t j = 0; j < message_count; ++j) {
                const google_protobuf_DescriptorProto* msg_proto = messages[j];
                upb_StringView msg_name_sv = google_protobuf_DescriptorProto_name(msg_proto);
                std::string msg_name(msg_name_sv.data, msg_name_sv.size);
                std::string full_msg_name = package_name + "::" + msg_name;
                content_ss << "Protobuf::ClassGenerator->register_class('" << full_msg_name << "');" << std::endl;
            }
            content_ss << std::endl;
        }


        content_ss << std::endl << "1;" << std::endl;
        std::string content = content_ss.str();

        google_protobuf_compiler_CodeGeneratorResponse_File* file =
            google_protobuf_compiler_CodeGeneratorResponse_add_file(response, &arena);
        google_protobuf_compiler_CodeGeneratorResponse_File_set_name(file, upb_StringView_FromStringView(module_path, &arena));
        google_protobuf_compiler_CodeGeneratorResponse_File_set_content(file, upb_StringView_FromStringView(content, &arena));
    }

    size_t response_size;
    const char* response_data = google_protobuf_compiler_CodeGeneratorResponse_serialize(response, &arena, &response_size);
    if (!response_data) {
        std::cerr << "Failed to serialize CodeGeneratorResponse." << std::endl;
        if (out_dir) free(out_dir);
        upb_DefPool_Free(pool);
        upb_Arena_Free(&arena);
        return 1;
    }
    std::cout.write(response_data, response_size);

    if (out_dir) free(out_dir);
    upb_DefPool_Free(pool);
    upb_Arena_Free(&arena);
    return 0;
}
