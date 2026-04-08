#include "perl_code_generator.h"
#include "upb/base/string_view.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <iterator>
#include <set>

// TODO: Move all helper functions and generation logic here from protoc-gen-perl-pb.cc

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

// ... Implementations of print_header, print_uses, etc. ...
// ... Implementations of helper functions ...

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
        result += segment;
        first = false;
    }
    return result;
}

void PerlCodeGenerator::print_header() {
    content_ss_ << "package " << package_name_ << ";" << std::endl;
    content_ss_ << R"(use strict;)" << std::endl;
    content_ss_ << R"(use warnings;)" << std::endl;
    content_ss_ << R"(use Protobuf::Message;)" << std::endl;
    content_ss_ << R"(use Protobuf::Internal qw(:all);)" << std::endl;
}

void PerlCodeGenerator::print_uses() {
    // TODO: Move logic for collecting uses here
    if (embed_descriptors_) {
        required_uses_.insert("use MIME::Base64;");
    }

    size_t message_count;
    const google_protobuf_DescriptorProto* const* messages =
        google_protobuf_FileDescriptorProto_message_type(file_proto_, &message_count);
    bool has_any_enums = false;
    size_t enum_count;
    google_protobuf_FileDescriptorProto_enum_type(file_proto_, &enum_count);
    if (enum_count > 0) has_any_enums = true;

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
        required_uses_.insert("use Const::Fast;");
    }

    for (const auto& use_stmt : required_uses_) {
        content_ss_ << use_stmt << std::endl;
    }
    content_ss_ << std::endl;
}

void PerlCodeGenerator::print_embedded_descriptor() {
    // TODO: Move embedded descriptor logic here
}

void PerlCodeGenerator::print_enums() {
    // TODO: Move top-level enum generation here
}

void PerlCodeGenerator::print_services() {
    // TODO: Move service generation here
}

void PerlCodeGenerator::print_messages() {
    // TODO: Move message generation loop here
}

void PerlCodeGenerator::print_message(const google_protobuf_DescriptorProto* msg_proto, const std::string& current_package) {
    // TODO: Move single message generation logic here
}

void PerlCodeGenerator::print_enum(const google_protobuf_EnumDescriptorProto* enum_proto, const std::string& parent_msg_name) {
    // TODO: Move single enum generation logic here
}
