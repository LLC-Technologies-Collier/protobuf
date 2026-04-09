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
#include <set>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <cctype>
#include <iterator>

// -- PerlCodeGenerator Class --
class PerlCodeGenerator {
public:
    PerlCodeGenerator(const google_protobuf_FileDescriptorProto* file_proto, upb_Arena* arena, bool embed_descriptors, bool generate_services);

    std::string generate();
    std::string get_package_name() const;

private:
    const google_protobuf_FileDescriptorProto* file_proto_;
    upb_Arena* arena_;
    bool embed_descriptors_;
    bool generate_services_;
    std::stringstream content_ss_;
    std::string package_name_;

    // String utilities
    static inline std::string capitalize(std::string s);
    static inline std::string to_camel_case(const std::string& s);
    static inline std::string base64_encode(const std::string& in);
    static inline std::string get_module_base_name(const std::string& package);
    static inline std::string get_perl_package_name(const std::string& proto_file_path, const std::string& package);
    static inline std::string proto_type_to_perl_module(const std::string& type_name);

    // Use statement management
    bool has_enums() const;
    bool is_type_defined_in_file(const std::string& module_name) const;
    void collect_field_type_uses(const google_protobuf_DescriptorProto* msg_proto);

    // Printing methods
    void print_header();
    void print_uses();
    void print_embedded_descriptor();
    void print_enums();
    void print_services();
    void print_messages();
    void print_message(const google_protobuf_DescriptorProto* msg_proto, const std::string& current_package);
    void print_enum(const google_protobuf_EnumDescriptorProto* enum_proto, const std::string& parent_msg_name);
    void register_message_and_nested(const google_protobuf_DescriptorProto* msg_proto, const std::string& current_package);

    std::set<std::string> required_uses_;
};

// -- String Utilities --
std::string PerlCodeGenerator::capitalize(std::string s) {
    if (!s.empty()) {
        s[0] = std::toupper(s[0]);
    }
    return s;
}

std::string PerlCodeGenerator::to_camel_case(const std::string& s) {
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

std::string PerlCodeGenerator::base64_encode(const std::string& in) {
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

std::string PerlCodeGenerator::get_module_base_name(const std::string& package) {
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

std::string PerlCodeGenerator::get_perl_package_name(const std::string& proto_file_path, const std::string& package) {
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

std::string PerlCodeGenerator::proto_type_to_perl_module(const std::string& type_name) {
    std::string module_name = type_name;
    if (!module_name.empty() && module_name[0] == '.') {
        module_name = module_name.substr(1);
    }

    std::string result = "";
    std::stringstream ss(module_name);
    std::string segment;
    bool first = true;
    while(std::getline(ss, segment, '.')) {
        if (!first) result += "::";
        result += segment; // Messages/Enums are already CamelCased in proto
        first = false;
    }
    return result;
}

// -- PerlCodeGenerator Methods --
PerlCodeGenerator::PerlCodeGenerator(const google_protobuf_FileDescriptorProto* file_proto, upb_Arena* arena, bool embed_descriptors, bool generate_services)
    : file_proto_(file_proto), arena_(arena), embed_descriptors_(embed_descriptors), generate_services_(generate_services) {
    upb_StringView proto_file_name_sv = google_protobuf_FileDescriptorProto_name(file_proto_);
    std::string proto_file_name(proto_file_name_sv.data, proto_file_name_sv.size);
    upb_StringView package_sv = google_protobuf_FileDescriptorProto_package(file_proto_);
    std::string package(package_sv.data, package_sv.size);
    package_name_ = get_perl_package_name(proto_file_name, package);
}

std::string PerlCodeGenerator::generate() {
    print_header();
    print_uses();
    if (embed_descriptors_) {
        print_embedded_descriptor();
    }
    print_enums();
    print_services();
    print_messages();

    content_ss_ << std::endl << "1;" << std::endl;
    return content_ss_.str();
}

void PerlCodeGenerator::print_header() {
    content_ss_ << "package " << package_name_ << ";" << std::endl;
    content_ss_ << R"(use strict;)" << std::endl;
    content_ss_ << R"(use warnings;)" << std::endl;
    content_ss_ << R"(use Protobuf::Message;)" << std::endl;
    content_ss_ << R"(use Protobuf::Internal qw(:all);)" << std::endl;
}

bool PerlCodeGenerator::has_enums() const {
    size_t enum_count;
    google_protobuf_FileDescriptorProto_enum_type(file_proto_, &enum_count);
    if (enum_count > 0) return true;

    size_t message_count;
    const google_protobuf_DescriptorProto* const* messages =
        google_protobuf_FileDescriptorProto_message_type(file_proto_, &message_count);
    if (message_count > 0) {
        for (size_t j = 0; j < message_count; ++j) {
            size_t nested_enum_count;
            google_protobuf_DescriptorProto_enum_type(messages[j], &nested_enum_count);
            if (nested_enum_count > 0) return true;
        }
    }
    return false;
}

void PerlCodeGenerator::collect_field_type_uses(const google_protobuf_DescriptorProto* msg_proto) {
    size_t field_count;
    const google_protobuf_FieldDescriptorProto* const* fields =
        google_protobuf_DescriptorProto_field(msg_proto, &field_count);
    for (size_t k = 0; k < field_count; ++k) {
        const google_protobuf_FieldDescriptorProto* field_proto = fields[k];
        google_protobuf_FieldDescriptorProto_Type type = (google_protobuf_FieldDescriptorProto_Type)google_protobuf_FieldDescriptorProto_type(field_proto);
        if (type == google_protobuf_FieldDescriptorProto_TYPE_MESSAGE || type == google_protobuf_FieldDescriptorProto_TYPE_ENUM) {
            upb_StringView type_name_sv = google_protobuf_FieldDescriptorProto_type_name(field_proto);
            std::string type_name(type_name_sv.data, type_name_sv.size);
            std::string module_name = proto_type_to_perl_module(type_name);
            
            if (!module_name.empty() && module_name.rfind(package_name_ + "::", 0) != 0) {
                if (!is_type_defined_in_file(module_name)) {
                     required_uses_.insert("use " + module_name + ";");
                }
            }
        }
    }
}

bool PerlCodeGenerator::is_type_defined_in_file(const std::string& module_name) const {
    size_t message_count;
    const google_protobuf_DescriptorProto* const* messages =
        google_protobuf_FileDescriptorProto_message_type(file_proto_, &message_count);
    for(size_t l = 0; l < message_count; ++l) {
        upb_StringView msg_name_sv = google_protobuf_DescriptorProto_name(messages[l]);
        std::string msg_name(msg_name_sv.data, msg_name_sv.size);
        if (package_name_ + "::" + msg_name == module_name) return true;
    }

    size_t top_enum_count;
    const google_protobuf_EnumDescriptorProto* const* top_enums =
        google_protobuf_FileDescriptorProto_enum_type(file_proto_, &top_enum_count);
     for (size_t l = 0; l < top_enum_count; ++l) {
        upb_StringView enum_name_sv = google_protobuf_EnumDescriptorProto_name(top_enums[l]);
        std::string enum_name(enum_name_sv.data, enum_name_sv.size);
        if (package_name_ + "::" + enum_name == module_name) return true;
    }
    return false;
}

void PerlCodeGenerator::print_uses() {
    if (embed_descriptors_) {
        required_uses_.insert("use MIME::Base64;");
    }
    if (has_enums()) {
        required_uses_.insert("use Const::Fast;");
    }

    size_t message_count;
    const google_protobuf_DescriptorProto* const* messages =
        google_protobuf_FileDescriptorProto_message_type(file_proto_, &message_count);
    for (size_t j = 0; j < message_count; ++j) {
        collect_field_type_uses(messages[j]);
    }

    for (const auto& use_stmt : required_uses_) {
        content_ss_ << use_stmt << std::endl;
    }
    content_ss_ << std::endl;
}

void PerlCodeGenerator::print_embedded_descriptor() {
    size_t serialized_fd_size;
    const char* serialized_fd_data = google_protobuf_FileDescriptorProto_serialize(file_proto_, arena_, &serialized_fd_size);
    if (!serialized_fd_data) {
         std::cerr << "Failed to serialize FileDescriptorProto for embedding" << std::endl;
         return;
    }
    std::string serialized_fd(serialized_fd_data, serialized_fd_size);
    std::string b64_descriptor = base64_encode(serialized_fd);

    content_ss_ << R"(BEGIN {)" << std::endl;
    content_ss_ << R"(    my $descriptor_b64 = <<'END_DESC';)" << std::endl;
    size_t chunk_size = 64;
    for (size_t k = 0; k < b64_descriptor.length(); k += chunk_size) {
        content_ss_ << b64_descriptor.substr(k, chunk_size) << std::endl;
    }
    content_ss_ << R"(END_DESC)" << std::endl;
    content_ss_ << R"(    Protobuf::DescriptorPool::get_generated_pool()->add_serialized_file()" << std::endl;
    content_ss_ << R"(        MIME::Base64::decode_base64(join("", grep { /\S/ } split(/?
/, $descriptor_b64))))" << std::endl;
    content_ss_ << R"(    );)" << std::endl;
    content_ss_ << R"(})" << std::endl << std::endl;
}

void PerlCodeGenerator::print_enums() {
    size_t enum_count;
    const google_protobuf_EnumDescriptorProto* const* enums =
        google_protobuf_FileDescriptorProto_enum_type(file_proto_, &enum_count);
    for (size_t j = 0; j < enum_count; ++j) {
        print_enum(enums[j], "");
    }
}

void PerlCodeGenerator::print_enum(const google_protobuf_EnumDescriptorProto* enum_proto, const std::string& parent_msg_name) {
    upb_StringView enum_name_sv = google_protobuf_EnumDescriptorProto_name(enum_proto);
    std::string enum_name(enum_name_sv.data, enum_name_sv.size);
    std::string prefix = parent_msg_name.empty() ? "" : parent_msg_name + "_";

    content_ss_ << "# Enum: " << (parent_msg_name.empty() ? enum_name : parent_msg_name + "::" + enum_name) << std::endl;
    size_t value_count;
    const google_protobuf_EnumValueDescriptorProto* const* values =
        google_protobuf_EnumDescriptorProto_value(enum_proto, &value_count);
    for (size_t k = 0; k < value_count; ++k) {
        const google_protobuf_EnumValueDescriptorProto* value_proto = values[k];
        upb_StringView value_name_sv = google_protobuf_EnumValueDescriptorProto_name(value_proto);
        std::string value_name(value_name_sv.data, value_name_sv.size);
        int32_t value_number = google_protobuf_EnumValueDescriptorProto_number(value_proto);
        content_ss_ << "const my $" << prefix << value_name << " => " << value_number << ";" << std::endl;
    }
    content_ss_ << std::endl;
}

void PerlCodeGenerator::print_services() {
    if (!generate_services_) return;

    size_t service_count;
    const google_protobuf_ServiceDescriptorProto* const* services =
        google_protobuf_FileDescriptorProto_service(file_proto_, &service_count);
    if (service_count > 0) {
        content_ss_ << "# Service definitions" << std::endl << std::endl;
        for (size_t j = 0; j < service_count; ++j) {
            const google_protobuf_ServiceDescriptorProto* service_proto = services[j];
            upb_StringView service_name_sv = google_protobuf_ServiceDescriptorProto_name(service_proto);
            std::string service_name(service_name_sv.data, service_name_sv.size);
            content_ss_ << "# Service: " << service_name << std::endl;

            size_t method_count;
            const google_protobuf_MethodDescriptorProto* const* methods =
                google_protobuf_ServiceDescriptorProto_method(service_proto, &method_count);
            for (size_t k = 0; k < method_count; ++k) {
                const google_protobuf_MethodDescriptorProto* method_proto = methods[k];
                upb_StringView method_name_sv = google_protobuf_MethodDescriptorProto_name(method_proto);
                std::string method_name(method_name_sv.data, method_name_sv.size);
                upb_StringView input_type_sv = google_protobuf_MethodDescriptorProto_input_type(method_proto);
                std::string input_type = proto_type_to_perl_module(std::string(input_type_sv.data, input_type_sv.size));
                upb_StringView output_type_sv = google_protobuf_MethodDescriptorProto_output_type(method_proto);
                std::string output_type = proto_type_to_perl_module(std::string(output_type_sv.data, output_type_sv.size));

                content_ss_ << "#   Method: " << method_name << "(" << input_type << ") returns (" << output_type << ")" << std::endl;
            }
            content_ss_ << std::endl;
        }
    }
}

void PerlCodeGenerator::print_messages() {
    size_t message_count;
    const google_protobuf_DescriptorProto* const* messages =
        google_protobuf_FileDescriptorProto_message_type(file_proto_, &message_count);
    if (message_count > 0) {
        content_ss_ << "# Message definitions" << std::endl << std::endl;
        for (size_t j = 0; j < message_count; ++j) {
            print_message(messages[j], package_name_);
        }
        content_ss_ << std::endl;
        content_ss_ << "# Register Messages" << std::endl;
        for (size_t j = 0; j < message_count; ++j) {
            register_message_and_nested(messages[j], package_name_);
        }
        content_ss_ << std::endl;
    }
}

void PerlCodeGenerator::register_message_and_nested(const google_protobuf_DescriptorProto* msg_proto, const std::string& current_package) {
    upb_StringView msg_name_sv = google_protobuf_DescriptorProto_name(msg_proto);
    std::string msg_name(msg_name_sv.data, msg_name_sv.size);
    std::string full_msg_name = current_package + "::" + msg_name;
    content_ss_ << "Protobuf::ClassGenerator->register_class('" << full_msg_name << "');" << std::endl;

    size_t nested_message_count;
    const google_protobuf_DescriptorProto* const* nested_messages =
        google_protobuf_DescriptorProto_nested_type(msg_proto, &nested_message_count);
    for (size_t k = 0; k < nested_message_count; ++k) {
        register_message_and_nested(nested_messages[k], full_msg_name);
    }
}

void PerlCodeGenerator::print_message(const google_protobuf_DescriptorProto* msg_proto, const std::string& current_package) {
    upb_StringView msg_name_sv = google_protobuf_DescriptorProto_name(msg_proto);
    std::string msg_name(msg_name_sv.data, msg_name_sv.size);
    std::string full_msg_name = current_package + "::" + msg_name;

    content_ss_ << "# === Message: " << full_msg_name << " ===" << std::endl;

    // Fields
    size_t field_count;
    const google_protobuf_FieldDescriptorProto* const* fields =
        google_protobuf_DescriptorProto_field(msg_proto, &field_count);
    if (field_count > 0) {
        content_ss_ << "    # Fields for " << msg_name << std::endl;
        for (size_t k = 0; k < field_count; ++k) {
            const google_protobuf_FieldDescriptorProto* field_proto = fields[k];
            upb_StringView field_name_sv = google_protobuf_FieldDescriptorProto_name(field_proto);
            std::string field_name(field_name_sv.data, field_name_sv.size);
            google_protobuf_FieldDescriptorProto_Type type = (google_protobuf_FieldDescriptorProto_Type)google_protobuf_FieldDescriptorProto_type(field_proto);
             upb_StringView type_name_sv = google_protobuf_FieldDescriptorProto_type_name(field_proto);
            std::string type_name(type_name_sv.data, type_name_sv.size);
            content_ss_ << "    # Field: " << field_name << " Type: " << type << " (" << type_name << ")" << std::endl;
        }
        content_ss_ << std::endl;
    }

    // Nested Enums
    size_t nested_enum_count;
    const google_protobuf_EnumDescriptorProto* const* nested_enums =
        google_protobuf_DescriptorProto_enum_type(msg_proto, &nested_enum_count);
    for (size_t k = 0; k < nested_enum_count; ++k) {
        print_enum(nested_enums[k], msg_name);
    }

    // Nested Messages
    size_t nested_message_count;
    const google_protobuf_DescriptorProto* const* nested_messages =
        google_protobuf_DescriptorProto_nested_type(msg_proto, &nested_message_count);
    for (size_t k = 0; k < nested_message_count; ++k) {
        print_message(nested_messages[k], full_msg_name);
    }

    // Extension Ranges
    size_t extension_range_count;
    const google_protobuf_DescriptorProto_ExtensionRange* const* extension_ranges =
        google_protobuf_DescriptorProto_extension_range(msg_proto, &extension_range_count);
    if (extension_range_count > 0) {
        content_ss_ << "    # Extension Ranges for " << msg_name << std::endl;
        for (size_t k = 0; k < extension_range_count; ++k) {
            const google_protobuf_DescriptorProto_ExtensionRange* range = extension_ranges[k];
            int32_t start = google_protobuf_DescriptorProto_ExtensionRange_start(range);
            int32_t end = google_protobuf_DescriptorProto_ExtensionRange_end(range) - 1;
            content_ss_ << "    Protobuf::ClassGenerator->register_extension_range('" << full_msg_name << "', " << start << ", " << end << ");" << std::endl;
        }
        content_ss_ << std::endl;
    }
}

std::string PerlCodeGenerator::get_package_name() const {
    return package_name_;
}

// -- main --

// Helper to create a upb_StringView from a std::string on a given arena
upb_StringView upb_StringView_FromStringView(const std::string& str, upb_Arena* arena) {
    char* data = (char*)upb_Arena_Malloc(arena, str.length());
    if (!data) return upb_StringView_FromString("");
    memcpy(data, str.data(), str.length());
    return upb_StringView_FromDataAndSize(data, str.length());
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

int main(int argc, char* argv[]) {
    upb_Arena arena;
    upb_Arena_Init(&arena, 0, NULL);

    std::string stdin_content;
    // Read all of stdin
    std::cin.seekg(0, std::ios::end);
    stdin_content.resize(std::cin.tellg());
    std::cin.seekg(0, std::ios::beg);
    std::cin.read(&stdin_content[0], stdin_content.size());

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

    size_t num_proto_files;
    const google_protobuf_FileDescriptorProto* const* proto_files =
        google_protobuf_compiler_CodeGeneratorRequest_proto_file(request, &num_proto_files);

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

        PerlCodeGenerator generator(proto_file, &arena, embed_descriptors, generate_services);
        std::string content = generator.generate();
        std::string module_path = package_name_to_module_path(generator.get_package_name());

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
        upb_Arena_Free(&arena);
        return 1;
    }
    std::cout.write(response_data, response_size);

    if (out_dir) free(out_dir);
    upb_Arena_Free(&arena);
    return 0;
}
