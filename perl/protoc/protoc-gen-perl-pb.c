#include "google/protobuf/compiler/plugin.pb.h"
#include "google/protobuf/descriptor.pb.h"
#include "google/protobuf/descriptor.upb.h"
#include "upb/reflection/def.h"
#include "upb/mem/arena.h"
#include "upb/base/status.h"
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

// Converts package and proto file name to Perl module name base

std::string get_module_base_name(const std::string& package, const std::string& proto_file) {
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

// Converts a proto file path to a Perl module file path
std::string proto_path_to_module_path(const std::string& proto_file_path, const std::string& package) {
    std::string package_path = package;
    std::replace(package_path.begin(), package_path.end(), '.', '/');

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

    return (package_path.empty() ? "" : package_path + "/") + proto_basename + ".pm";
}

// TODO: Implement the main logic for protoc-gen-perl-pb
// TODO: Process FileDescriptorProtos using upb_DefPool
// TODO: Generate Perl module content
// TODO: Write CodeGeneratorResponse to stdout

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

int main(int argc, char* argv[]) {
    google::protobuf::compiler::CodeGeneratorRequest request;
    if (!request.ParseFromIstream(&std::cin)) {
        std::cerr << "Failed to parse CodeGeneratorRequest from stdin." << std::endl;
        return 1;
    }

    if (!request.parameter().empty()) {
        parse_options(request.parameter());
    }

    upb_DefPool* pool = upb_DefPool_New();
    if (!pool) {
        std::cerr << "Failed to create upb_DefPool." << std::endl;
        return 1;
    }

    upb_Arena arena;
    upb_Arena_Init(&arena, 0, NULL);

    for (int i = 0; i < request.proto_file_size(); ++i) {
        std::string serialized_fd;
        if (!request.proto_file(i).SerializeToString(&serialized_fd)) {
            std::cerr << "Failed to serialize FileDescriptorProto: " << request.proto_file(i).name() << std::endl;
            upb_Arena_Free(&arena);
            upb_DefPool_Free(pool);
            return 1;
        }

        google_protobuf_FileDescriptorProto* upb_fd = google_protobuf_FileDescriptorProto_parse(serialized_fd.data(), serialized_fd.size(), &arena);
        if (!upb_fd) {
            std::cerr << "Failed to parse FileDescriptorProto in UPB: " << request.proto_file(i).name() << std::endl;
            upb_Arena_Free(&arena);
            upb_DefPool_Free(pool);
            return 1;
        }

        upb_Status status;
        upb_Status_Clear(&status);
        if (!upb_DefPool_AddFile(pool, upb_fd, &status)) {
            std::cerr << "Failed to add FileDescriptorProto to pool: "
                      << request.proto_file(i).name() << " - "
                      << upb_Status_ErrorMessage(&status) << std::endl;
            upb_Arena_Free(&arena);
            upb_DefPool_Free(pool);
            return 1;
        }
    }
    upb_Arena_Free(&arena);

    std::cerr << "protoc-gen-perl-pb plugin not fully implemented yet." << std::endl;
    fprintf(stderr, "Embed descriptors: %s
", (embed_descriptors ? "true" : "false"));
    fprintf(stderr, "Generate services: %s
", (generate_services ? "true" : "false"));
    if (out_dir) {
        fprintf(stderr, "Output directory: %s
", out_dir);
        free(out_dir);
    }

    google::protobuf::compiler::CodeGeneratorResponse response;

    for (int i = 0; i < request.proto_file_size(); ++i) {
        const google::protobuf::FileDescriptorProto& proto_file = request.proto_file(i);
        bool generate = false;
        for (int j = 0; j < request.file_to_generate_size(); ++j) {
            if (request.file_to_generate(j) == proto_file.name()) {
                generate = true;
                break;
            }
        }
        if (!generate) {
            continue;
        }

        std::string module_path = proto_path_to_module_path(proto_file.name(), proto_file.package());

        std::string package_name = module_path;
        size_t last_dot = package_name.find_last_of('.');
        if (last_dot != std::string::npos) {
            package_name = package_name.substr(0, last_dot);
        }
        std::replace(package_name.begin(), package_name.end(), '/', ':');

        std::string content = "package " + package_name + ";\n";
        content += "use strict;\n";
        content += "use warnings;\n";
        content += "use Protobuf::Message;\n";
        content += "use Protobuf::Internal qw(:all);\n";
        bool has_enums = proto_file.enum_type_size() > 0;
        if (has_enums) {
            content += "use Const::Fast;\n";
        }
        if (embed_descriptors) {
            content += "use MIME::Base64;\n";
        }
        content += "\n";

        if (embed_descriptors) {
            std::string serialized_fd;
            if (!proto_file.SerializeToString(&serialized_fd)) {
                 google::protobuf::compiler::CodeGeneratorResponse err_response;
                 err_response.set_error("Failed to serialize FileDescriptorProto: " + proto_file.name());
                 err_response.SerializeToOstream(&std::cout);
                 return 1;
            }
            std::string b64_descriptor = base64_encode(serialized_fd);

            content += "BEGIN {\n";
            content += "    my $descriptor_b64 = <<'END_DESC';\n";
            size_t chunk_size = 64;
            for (size_t i = 0; i < b64_descriptor.length(); i += chunk_size) {
                content += b64_descriptor.substr(i, chunk_size) + "\n";
            }
            content += "END_DESC\n";
            content += "    Protobuf::DescriptorPool::get_generated_pool()->add_serialized_file(\n";
            content += "        MIME::Base64::decode_base64(join(\"\", grep { /\S/ } split(/\r?\n/, $descriptor_b64)))\n";
            content += "    );\n";
            content += "}\n\n";
        }

        // Message Class Stubs
        if (!embed_descriptors) {
            for (int j = 0; j < proto_file.message_type_size(); ++j) {
                const google::protobuf::DescriptorProto& msg_proto = proto_file.message_type(j);
                content += "# Protobuf::ClassGenerator->register_class('" + package_name + "::" + msg_proto.name() + "');\n";
            }
            if (proto_file.message_type_size() > 0) {
                content += "\n";
            }
        }

        // Generate Enums
        if (has_enums) {
            for (int j = 0; j < proto_file.enum_type_size(); ++j) {
                const google::protobuf::EnumDescriptorProto& enum_proto = proto_file.enum_type(j);
                // TODO: Prefix enum values with enum name?
                for (int k = 0; k < enum_proto.value_size(); ++k) {
                    const google::protobuf::EnumValueDescriptorProto& value_proto = enum_proto.value(k);
                    content += "const my $" + value_proto.name() + " => " + std::to_string(value_proto.number()) + ";\n";
                }
                content += "\n";
            }
        }

        // Generate Services
        if (generate_services && proto_file.service_size() > 0) {
            content += "# Service definitions would go here.\n\n";
            for (int j = 0; j < proto_file.service_size(); ++j) {
                const google::protobuf::ServiceDescriptorProto& service_proto = proto_file.service(j);
                content += "# Service: " + service_proto.name() + "\n";
                for (int k = 0; k < service_proto.method_size(); ++k) {
                    const google::protobuf::MethodDescriptorProto& method_proto = service_proto.method(k);
                    content += "#   Method: " + method_proto.name() + "\n";
                }
                content += "\n";
            }
        }

        content += "\n1;\n";

        google::protobuf::compiler::CodeGeneratorResponse::File* file = response.add_file();
        file->set_name(module_path);
        file->set_content(content);
    }

    response.SerializeToOstream(&std::cout);

    upb_DefPool_Free(pool);
    return 0;
}
