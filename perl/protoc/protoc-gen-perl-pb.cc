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
#include <map>

// -- PerlCodeGenerator Class --
class PerlCodeGenerator {
public:
    PerlCodeGenerator(const google_protobuf_FileDescriptorProto* file_proto, upb_Arena* arena, bool embed_descriptors, bool generate_services);

    std::string generate();
    std::string generate_types();
    std::string get_package_name() const;
    static void register_all_types(const google_protobuf_FileDescriptorProto* file_proto, upb_Arena* arena);
    static void register_types_recursively(const google_protobuf_DescriptorProto* msg_proto, const std::string& full_proto_name, const std::string& module_name);

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
    void print_types_recursively(const google_protobuf_DescriptorProto* msg_proto, const std::string& current_package, std::stringstream& ss);
    void print_enum_types(const google_protobuf_EnumDescriptorProto* enum_proto, const std::string& current_package, std::stringstream& ss);
    void print_enum(const google_protobuf_EnumDescriptorProto* enum_proto, const std::string& parent_msg_name);
    void register_message_and_nested(const google_protobuf_DescriptorProto* msg_proto, const std::string& current_package);

    static std::map<std::string, std::string> type_to_module_map;

    std::set<std::string> required_uses_;
};

std::map<std::string, std::string> PerlCodeGenerator::type_to_module_map;

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
    std::string full_name = type_name;
    if (!full_name.empty() && full_name[0] != '.') {
        full_name = "." + full_name;
    }
    
    auto it = type_to_module_map.find(full_name);
    if (it != type_to_module_map.end()) {
        return it->second;
    }

    // Fallback logic
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
        result += segment;
        first = false;
    }
    return result;
}

void PerlCodeGenerator::register_all_types(const google_protobuf_FileDescriptorProto* file_proto, upb_Arena* arena) {
    upb_StringView proto_file_name_sv = google_protobuf_FileDescriptorProto_name(file_proto);
    std::string proto_file_name(proto_file_name_sv.data, proto_file_name_sv.size);
    
    upb_StringView package_sv = google_protobuf_FileDescriptorProto_package(file_proto);
    std::string package(package_sv.data, package_sv.size);
    std::string package_prefix = package.empty() ? "." : "." + package + ".";

    std::string module_name = get_perl_package_name(proto_file_name, package);

    // Top-level messages
    size_t message_count;
    const google_protobuf_DescriptorProto* const* messages =
        google_protobuf_FileDescriptorProto_message_type(file_proto, &message_count);
    for (size_t j = 0; j < message_count; ++j) {
        upb_StringView name_sv = google_protobuf_DescriptorProto_name(messages[j]);
        std::string name(name_sv.data, name_sv.size);
        std::string full_proto_name = package_prefix + name;
        register_types_recursively(messages[j], full_proto_name, module_name);
    }

    // Top-level enums
    size_t enum_count;
    const google_protobuf_EnumDescriptorProto* const* enums =
        google_protobuf_FileDescriptorProto_enum_type(file_proto, &enum_count);
    for (size_t j = 0; j < enum_count; ++j) {
        upb_StringView name_sv = google_protobuf_EnumDescriptorProto_name(enums[j]);
        std::string full_name = package_prefix + std::string(name_sv.data, name_sv.size);
        type_to_module_map[full_name] = module_name;
    }
}

void PerlCodeGenerator::register_types_recursively(const google_protobuf_DescriptorProto* msg_proto, const std::string& full_proto_name, const std::string& module_name) {
    type_to_module_map[full_proto_name] = module_name;

    // Nested messages
    size_t nested_message_count;
    const google_protobuf_DescriptorProto* const* nested_messages =
        google_protobuf_DescriptorProto_nested_type(msg_proto, &nested_message_count);
    for (size_t k = 0; k < nested_message_count; ++k) {
        upb_StringView sub_name_sv = google_protobuf_DescriptorProto_name(nested_messages[k]);
        std::string sub_full_name = full_proto_name + "." + std::string(sub_name_sv.data, sub_name_sv.size);
        register_types_recursively(nested_messages[k], sub_full_name, module_name);
    }

    // Nested enums
    size_t nested_enum_count;
    const google_protobuf_EnumDescriptorProto* const* nested_enums =
        google_protobuf_DescriptorProto_enum_type(msg_proto, &nested_enum_count);
    for (size_t k = 0; k < nested_enum_count; ++k) {
        upb_StringView sub_name_sv = google_protobuf_EnumDescriptorProto_name(nested_enums[k]);
        std::string sub_full_name = full_proto_name + "." + std::string(sub_name_sv.data, sub_name_sv.size);
        type_to_module_map[sub_full_name] = module_name;
    }
}

// -- PerlCodeGenerator Methods --
PerlCodeGenerator::PerlCodeGenerator(const google_protobuf_FileDescriptorProto* file_proto, upb_Arena* arena, bool embed_descriptors, bool generate_services)
    : file_proto_(file_proto), arena_(arena), embed_descriptors_(embed_descriptors), generate_services_(generate_services) {
    upb_StringView proto_file_name_sv = google_protobuf_FileDescriptorProto_name(file_proto_);
    std::string proto_file_name(proto_file_name_sv.data, proto_file_name_sv.size);
    std::cerr << "--- DEBUG: PerlCodeGenerator() for: " << proto_file_name << std::endl;

    upb_StringView package_sv = google_protobuf_FileDescriptorProto_package(file_proto_);
    std::string package(package_sv.data, package_sv.size);
    package_name_ = get_perl_package_name(proto_file_name, package);
}

std::string PerlCodeGenerator::generate() {
    upb_StringView proto_file_name_sv = google_protobuf_FileDescriptorProto_name(file_proto_);
    std::string proto_file_name(proto_file_name_sv.data, proto_file_name_sv.size);
    std::cerr << "--- DEBUG: generate() called for: " << proto_file_name << std::endl;

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
    content_ss_ << std::endl;
    content_ss_ << "use strict;" << std::endl;
    content_ss_ << "use warnings;" << std::endl;
    content_ss_ << std::endl;
    content_ss_ << "our $VERSION = '0.01';" << std::endl;
    content_ss_ << std::endl;
    content_ss_ << "use Protobuf::Message;" << std::endl;
    content_ss_ << "use Protobuf::DescriptorPool;" << std::endl;
    content_ss_ << "use Protobuf::Internal qw(:all);" << std::endl;
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
            
            if (!module_name.empty() && !is_type_defined_in_file(module_name)) {
                 required_uses_.insert("use " + module_name + ";");
            }
        }
    }
}

bool PerlCodeGenerator::is_type_defined_in_file(const std::string& module_name) const {
    return module_name == package_name_;
}

void PerlCodeGenerator::print_uses() {
    if (embed_descriptors_) {
        required_uses_.insert("use MIME::Base64;");
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
    size_t serialized_fd_size = 0;
    const char* serialized_fd_data = google_protobuf_FileDescriptorProto_serialize(file_proto_, arena_, &serialized_fd_size);
    if (!serialized_fd_data) {
         std::cerr << "--- ERROR: Failed to serialize FileDescriptorProto for embedding" << std::endl;
         return;
    }

    std::string serialized_fd(serialized_fd_data, serialized_fd_size);
    std::string b64_descriptor = base64_encode(serialized_fd);

    content_ss_ << "BEGIN {" << std::endl;
    content_ss_ << "    my $descriptor_b64 = <<'EOF';" << std::endl;
    for (size_t i = 0; i < b64_descriptor.length(); i += 76) {
        content_ss_ << b64_descriptor.substr(i, 76) << std::endl;
    }
    content_ss_ << "EOF" << std::endl;
    content_ss_ << "    Protobuf::DescriptorPool->generated_pool->add_serialized_file(MIME::Base64::decode_base64($descriptor_b64));" << std::endl;
    content_ss_ << "}" << std::endl << std::endl;
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
    std::string full_enum_name = package_name_ + "::" + (parent_msg_name.empty() ? "" : parent_msg_name + "::") + enum_name;

    content_ss_ << "package " << full_enum_name << ";" << std::endl;
    size_t value_count;
    const google_protobuf_EnumValueDescriptorProto* const* values =
        google_protobuf_EnumDescriptorProto_value(enum_proto, &value_count);
    for (size_t k = 0; k < value_count; ++k) {
        const google_protobuf_EnumValueDescriptorProto* value_proto = values[k];
        upb_StringView value_name_sv = google_protobuf_EnumValueDescriptorProto_name(value_proto);
        std::string value_name(value_name_sv.data, value_name_sv.size);
        int32_t value_number = google_protobuf_EnumValueDescriptorProto_number(value_proto);
        content_ss_ << "sub " << value_name << " { " << value_number << " }" << std::endl;
    }
    content_ss_ << "package " << package_name_ << ";" << std::endl << std::endl;
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

std::string PerlCodeGenerator::generate_types() {
    std::stringstream ss;
    ss << "package " << package_name_ << "::Types;" << std::endl << std::endl;
    ss << "use strict;" << std::endl;
    ss << "use warnings;" << std::endl << std::endl;
    ss << "use Type::Library -base;" << std::endl;
    ss << "use Type::Utils -all;" << std::endl;
    ss << "use Types::Standard -types;" << std::endl << std::endl;

    size_t enum_count;
    const google_protobuf_EnumDescriptorProto* const* enums =
        google_protobuf_FileDescriptorProto_enum_type(file_proto_, &enum_count);
    for (size_t j = 0; j < enum_count; ++j) {
        print_enum_types(enums[j], package_name_, ss);
    }

    size_t message_count;
    const google_protobuf_DescriptorProto* const* messages =
        google_protobuf_FileDescriptorProto_message_type(file_proto_, &message_count);
    for (size_t j = 0; j < message_count; ++j) {
        print_types_recursively(messages[j], package_name_, ss);
    }

    ss << "1;" << std::endl;
    return ss.str();
}

void PerlCodeGenerator::print_types_recursively(const google_protobuf_DescriptorProto* msg_proto, const std::string& current_package, std::stringstream& ss) {
    upb_StringView msg_name_sv = google_protobuf_DescriptorProto_name(msg_proto);
    std::string msg_name(msg_name_sv.data, msg_name_sv.size);
    std::string full_msg_name = current_package + "::" + msg_name;

    ss << "declare '" << msg_name << "'," << std::endl;
    ss << "    as InstanceOf['" << full_msg_name << "'];" << std::endl << std::endl;

    ss << "coerce '" << msg_name << "'," << std::endl;
    ss << "    from HashRef, via { '" << full_msg_name << "'->new($_) };" << std::endl << std::endl;

    size_t nested_enum_count;
    const google_protobuf_EnumDescriptorProto* const* nested_enums =
        google_protobuf_DescriptorProto_enum_type(msg_proto, &nested_enum_count);
    for (size_t k = 0; k < nested_enum_count; ++k) {
        print_enum_types(nested_enums[k], full_msg_name, ss);
    }

    size_t nested_message_count;
    const google_protobuf_DescriptorProto* const* nested_messages =
        google_protobuf_DescriptorProto_nested_type(msg_proto, &nested_message_count);
    for (size_t k = 0; k < nested_message_count; ++k) {
        print_types_recursively(nested_messages[k], full_msg_name, ss);
    }
}

void PerlCodeGenerator::print_enum_types(const google_protobuf_EnumDescriptorProto* enum_proto, const std::string& current_package, std::stringstream& ss) {
    upb_StringView enum_name_sv = google_protobuf_EnumDescriptorProto_name(enum_proto);
    std::string enum_name(enum_name_sv.data, enum_name_sv.size);
    // std::string full_enum_name = current_package + "::" + enum_name;

    ss << "declare '" << enum_name << "'," << std::endl;
    ss << "    as Int;" << std::endl << std::endl;
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

bool embed_descriptors = true;
bool generate_services = false;

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
        } else if (part == "no_embed_descriptors") {
            embed_descriptors = false;
        } else if (part.rfind("embed_descriptors=", 0) == 0) {
            std::string val = part.substr(18);
            embed_descriptors = (val == "true" || val == "1" || val == "yes");
        } else if (part == "generate_services") {
            generate_services = true;
        } else if (part.rfind("generate_services=", 0) == 0) {
            std::string val = part.substr(18);
            generate_services = (val == "true" || val == "1" || val == "yes");
        }
    }
}

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "This is a protoc plugin. It is intended to be called by protoc, not run manually." << std::endl;
    std::cout << "Example: protoc --plugin=protoc-gen-perl-pb=" << program_name << " --perl-pb_out=. foo.proto" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help            Show this help message and exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Plugin Options (passed via --perl-pb_opt=...):" << std::endl;
    std::cout << "  embed_descriptors     Embed Base64-encoded descriptors in generated .pm files (default: true)" << std::endl;
    std::cout << "  generate_services     Generate (placeholder) service stubs (default: false)" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        }
    }

    if (isatty(0)) {
        print_usage(argv[0]);
        return 0;
    }

    upb_Arena* arena = upb_Arena_New();
    if (!arena) {
        std::cerr << "--- ERROR: Failed to create arena!" << std::endl;
        return 1;
    }
    std::cerr << "--- DEBUG: Arena created ---" << std::endl;

    std::string stdin_content;
    try {
        std::cin >> std::noskipws;
        std::istream_iterator<char> it(std::cin);
        std::istream_iterator<char> end;
        stdin_content.assign(it, end);
    } catch (const std::exception& e) {
        std::cerr << "--- ERROR: Exception while reading stdin: " << e.what() << std::endl;
        upb_Arena_Free(arena);
        return 1;
    }
    std::cerr << "--- DEBUG: Read " << stdin_content.length() << " bytes from stdin." << std::endl;

    if (stdin_content.empty()) {
        std::cerr << "--- DEBUG: Empty stdin, exiting gracefully." << std::endl;
        upb_Arena_Free(arena);
        return 0;
    }

    google_protobuf_compiler_CodeGeneratorRequest* request =
        google_protobuf_compiler_CodeGeneratorRequest_parse(stdin_content.data(), stdin_content.size(), arena);

    if (!request) {
        std::cerr << "--- ERROR: Failed to parse CodeGeneratorRequest from stdin." << std::endl;
        upb_Arena_Free(arena);
        return 1;
    }
    std::cerr << "--- DEBUG: CodeGeneratorRequest parsed successfully." << std::endl;

    // Parse options
    upb_StringView options_sv = google_protobuf_compiler_CodeGeneratorRequest_parameter(request);
    if (options_sv.size > 0) {
        parse_options(std::string(options_sv.data, options_sv.size));
    }

    google_protobuf_compiler_CodeGeneratorResponse* response = google_protobuf_compiler_CodeGeneratorResponse_new(arena);

    size_t file_to_generate_count;
    const upb_StringView* files_to_generate = google_protobuf_compiler_CodeGeneratorRequest_file_to_generate(request, &file_to_generate_count);

    size_t proto_file_count;
    const google_protobuf_FileDescriptorProto* const* proto_files = google_protobuf_compiler_CodeGeneratorRequest_proto_file(request, &proto_file_count);

    std::cerr << "--- DEBUG: Generating " << file_to_generate_count << " files." << std::endl;

    // First pass: Register all types across all files
    for (size_t i = 0; i < proto_file_count; ++i) {
        PerlCodeGenerator::register_all_types(proto_files[i], arena);
    }

    // Second pass: Generate files
    for (size_t i = 0; i < file_to_generate_count; ++i) {
        std::string file_name(files_to_generate[i].data, files_to_generate[i].size);
        
        // Find the FileDescriptorProto for this file
        const google_protobuf_FileDescriptorProto* file_proto = nullptr;
        for (size_t j = 0; j < proto_file_count; ++j) {
            upb_StringView name_sv = google_protobuf_FileDescriptorProto_name(proto_files[j]);
            if (std::string(name_sv.data, name_sv.size) == file_name) {
                file_proto = proto_files[j];
                break;
            }
        }

        if (file_proto) {
            PerlCodeGenerator generator(file_proto, arena, embed_descriptors, generate_services);

            // Generate main module
            std::string content = generator.generate();
            std::string package_name = generator.get_package_name();
            std::string output_file = package_name_to_module_path(package_name);

            google_protobuf_compiler_CodeGeneratorResponse_File* file_res =
                google_protobuf_compiler_CodeGeneratorResponse_add_file(response, arena);

            google_protobuf_compiler_CodeGeneratorResponse_File_set_name(file_res, upb_StringView_FromStringView(output_file, arena));
            google_protobuf_compiler_CodeGeneratorResponse_File_set_content(file_res, upb_StringView_FromStringView(content, arena));

            std::cerr << "--- DEBUG: Generated: " << output_file << std::endl;

            // Generate Types module
            std::string types_content = generator.generate_types();
            std::string types_package = package_name + "::Types";
            std::string types_output_file = package_name_to_module_path(types_package);

            google_protobuf_compiler_CodeGeneratorResponse_File* types_file_res =
                google_protobuf_compiler_CodeGeneratorResponse_add_file(response, arena);

            google_protobuf_compiler_CodeGeneratorResponse_File_set_name(types_file_res, upb_StringView_FromStringView(types_output_file, arena));
            google_protobuf_compiler_CodeGeneratorResponse_File_set_content(types_file_res, upb_StringView_FromStringView(types_content, arena));

            std::cerr << "--- DEBUG: Generated: " << types_output_file << std::endl;
        }
 else {
            std::cerr << "--- ERROR: Could not find FileDescriptorProto for: " << file_name << std::endl;
        }
    }

    // Serialize response
    size_t response_size;
    char* response_data = google_protobuf_compiler_CodeGeneratorResponse_serialize(response, arena, &response_size);
    if (response_data) {
        std::cout.write(response_data, response_size);
        std::cerr << "--- DEBUG: Response serialized and written, size: " << response_size << std::endl;
    } else {
        std::cerr << "--- ERROR: Failed to serialize CodeGeneratorResponse!" << std::endl;
    }

    upb_Arena_Free(arena);
    std::cerr << "--- DEBUG: Arena freed ---" << std::endl;
    return 0;
}
