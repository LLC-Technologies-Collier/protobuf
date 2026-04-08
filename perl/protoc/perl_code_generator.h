#ifndef PERL_CODE_GENERATOR_H
#define PERL_CODE_GENERATOR_H

#include "google/protobuf/compiler/plugin.upb.h"
#include "google/protobuf/descriptor.upb.h"
#include "upb/reflection/def.h"
#include "upb/mem/arena.h"
#include <string>
#include <sstream>
#include <vector>
#include <set>

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
    std::string capitalize(std::string s);
    std::string to_camel_case(const std::string& s);
    std::string base64_encode(const std::string& in);
    std::string get_module_base_name(const std::string& package);
    std::string get_perl_package_name(const std::string& proto_file_path, const std::string& package);
    std::string proto_type_to_perl_module(const std::string& type_name);

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

#endif // PERL_CODE_GENERATOR_H
