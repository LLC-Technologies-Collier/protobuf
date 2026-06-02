#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <cstring>

#include "upb/mem/arena.h"
#include "upb/reflection/def.h"
#include "upb/reflection/file_def.h"
#include "upb/reflection/message_def.h"
#include "upb/reflection/enum_def.h"
#include "upb/reflection/field_def.h"
#include "upb/reflection/service_def.h"
#include "upb/reflection/method_def.h"
#include "upb/reflection/def_pool.h"
#include "upb/mini_table/message.h"
#include "upb/mini_table/field.h"
#include "upb/mini_table/enum.h"
#include "upb/util/def_to_proto.h"
#include "upb/wire/encode.h"

#include "upb/reflection/cmake/google/protobuf/descriptor.upb.h"
#include "google/protobuf/compiler/plugin.upb.h"

// -- Proto Utils --

// Converts a string to CamelCase
std::string to_camel_case(const std::string& s) {
    std::string res;
    bool next_upper = true;
    for (char c : s) {
        if (c == '_') {
            next_upper = true;
        } else {
            if (next_upper) {
                res += (char)toupper(c);
                next_upper = false;
            } else {
                res += c;
            }
        }
    }
    return res;
}

// Converts a CamelCase string to snake_case
std::string to_snake_case(const std::string& s) {
    std::string res;
    for (size_t i = 0; i < s.length(); i++) {
        char c = s[i];
        if (isupper(c)) {
            if (i > 0 && s[i-1] != '_') {
                res += '_';
            }
            res += (char)tolower(c);
        } else {
            res += c;
        }
    }
    return res;
}

// Extracts the default host for standard Google Cloud APIs based on the package prefix
std::string get_default_host(const std::string& proto_pkg) {
    if (proto_pkg.find("google.cloud.") == 0) {
        size_t start = 13;
        size_t end = proto_pkg.find('.', start);
        if (end != std::string::npos) {
            return proto_pkg.substr(start, end - start) + ".googleapis.com:443";
        }
    } else if (proto_pkg.find("google.") == 0) {
        size_t start = 7;
        size_t end = proto_pkg.find('.', start);
        if (end != std::string::npos) {
            return proto_pkg.substr(start, end - start) + ".googleapis.com:443";
        }
    }
    return "localhost:443";
}

// Capitalizes each segment of a package name (e.g. foo.bar -> Foo.Bar)
std::string capitalize_package(const std::string& s) {
    std::string res;
    std::stringstream ss(s);
    std::string segment;
    bool first = true;
    while (std::getline(ss, segment, '.')) {
        if (!first) res += "::";
        if (!segment.empty()) {
            segment[0] = (char)toupper(segment[0]);
        }
        res += segment;
        first = false;
    }
    return res;
}

// -- Generator Class --

class PerlCodeGenerator {
public:
    PerlCodeGenerator(const google_protobuf_compiler_CodeGeneratorRequest* request, upb_Arena* arena)
        : request_(request), arena_(arena) {
        build_package_map();
        build_type_map();
    }

    google_protobuf_compiler_CodeGeneratorResponse* generate();

private:
    void build_package_map();
    void build_type_map();
    void register_types_recursively(const google_protobuf_DescriptorProto* msg_proto, const std::string& proto_prefix, const std::string& perl_prefix);
    std::string resolve_perl_class(const std::string& proto_type);
    std::string get_perl_package(const google_protobuf_FileDescriptorProto* file_proto);

    std::string generate_module(const google_protobuf_FileDescriptorProto* file_proto);
    std::string generate_types(const google_protobuf_FileDescriptorProto* file_proto);
    
    void print_types_recursively(const google_protobuf_DescriptorProto* msg_proto, const std::string& current_package, std::stringstream& ss);
    void print_message_types(const google_protobuf_DescriptorProto* msg_proto, const std::string& current_package, std::stringstream& ss);
    void print_enum_types(const google_protobuf_EnumDescriptorProto* enum_proto, const std::string& current_package, std::stringstream& ss);

    upb_StringView arena_copy(const std::string& s) {
        char* buf = (char*)upb_Arena_Malloc(arena_, s.length());
        memcpy(buf, s.data(), s.length());
        return upb_StringView_FromDataAndSize(buf, s.length());
    }

    const google_protobuf_compiler_CodeGeneratorRequest* request_;
    upb_Arena* arena_;
    std::map<std::string, std::string> file_to_package_;
    std::map<std::string, std::string> type_to_perl_class_;
    std::string package_name_;
};

void PerlCodeGenerator::build_package_map() {
    size_t file_count;
    const google_protobuf_FileDescriptorProto* const* files = 
        google_protobuf_compiler_CodeGeneratorRequest_proto_file(request_, &file_count);

    for (size_t i = 0; i < file_count; i++) {
        upb_StringView name_sv = google_protobuf_FileDescriptorProto_name(files[i]);
        std::string name(name_sv.data, name_sv.size);
        file_to_package_[name] = get_perl_package(files[i]);
    }
}

void PerlCodeGenerator::register_types_recursively(const google_protobuf_DescriptorProto* msg_proto, const std::string& proto_prefix, const std::string& perl_prefix) {
    upb_StringView name_sv = google_protobuf_DescriptorProto_name(msg_proto);
    std::string name(name_sv.data, name_sv.size);

    std::string full_proto = proto_prefix + "." + name;
    std::string full_perl = perl_prefix + "::" + name;

    type_to_perl_class_[full_proto] = full_perl;

    size_t nested_count;
    const google_protobuf_DescriptorProto* const* nested = 
        google_protobuf_DescriptorProto_nested_type(msg_proto, &nested_count);
    for (size_t i = 0; i < nested_count; i++) {
        register_types_recursively(nested[i], full_proto, full_perl);
    }
}

void PerlCodeGenerator::build_type_map() {
    size_t file_count;
    const google_protobuf_FileDescriptorProto* const* files = 
        google_protobuf_compiler_CodeGeneratorRequest_proto_file(request_, &file_count);

    for (size_t i = 0; i < file_count; i++) {
        upb_StringView name_sv = google_protobuf_FileDescriptorProto_name(files[i]);
        std::string name(name_sv.data, name_sv.size);
        std::string perl_pkg = file_to_package_[name];

        upb_StringView pkg_sv = google_protobuf_FileDescriptorProto_package(files[i]);
        std::string proto_pkg = "." + std::string(pkg_sv.data, pkg_sv.size);

        size_t msg_count;
        const google_protobuf_DescriptorProto* const* messages = 
            google_protobuf_FileDescriptorProto_message_type(files[i], &msg_count);
        for (size_t j = 0; j < msg_count; j++) {
            register_types_recursively(messages[j], proto_pkg, perl_pkg);
        }
    }
}

std::string PerlCodeGenerator::resolve_perl_class(const std::string& proto_type) {
    if (type_to_perl_class_.count(proto_type)) {
        return type_to_perl_class_[proto_type];
    }
    std::string s = proto_type;
    if (s[0] == '.') s = s.substr(1);
    return capitalize_package(s);
}

std::string PerlCodeGenerator::get_perl_package(const google_protobuf_FileDescriptorProto* file_proto) {
    upb_StringView name_sv = google_protobuf_FileDescriptorProto_name(file_proto);
    std::string name(name_sv.data, name_sv.size);

    upb_StringView pkg_sv = google_protobuf_FileDescriptorProto_package(file_proto);
    std::string pkg(pkg_sv.data, pkg_sv.size);
    
    std::string file_segment = name;
    size_t last_slash = file_segment.find_last_of('/');
    if (last_slash != std::string::npos) file_segment = file_segment.substr(last_slash + 1);
    size_t dot = file_segment.find_last_of('.');
    if (dot != std::string::npos) file_segment = file_segment.substr(0, dot);
    file_segment = to_camel_case(file_segment);

    std::string base_package = capitalize_package(pkg);
    if (!base_package.empty()) {
        return base_package + "::" + file_segment;
    } else {
        return file_segment;
    }
}

google_protobuf_compiler_CodeGeneratorResponse* PerlCodeGenerator::generate() {
    google_protobuf_compiler_CodeGeneratorResponse* response = 
        google_protobuf_compiler_CodeGeneratorResponse_new(arena_);
    google_protobuf_compiler_CodeGeneratorResponse_set_supported_features(response, 
        google_protobuf_compiler_CodeGeneratorResponse_FEATURE_PROTO3_OPTIONAL);

    size_t gen_file_count;
    const upb_StringView* gen_files = 
        google_protobuf_compiler_CodeGeneratorRequest_file_to_generate(request_, &gen_file_count);

    size_t all_file_count;
    const google_protobuf_FileDescriptorProto* const* all_files = 
        google_protobuf_compiler_CodeGeneratorRequest_proto_file(request_, &all_file_count);

    for (size_t i = 0; i < gen_file_count; i++) {
        std::string name(gen_files[i].data, gen_files[i].size);
        
        const google_protobuf_FileDescriptorProto* file_proto = nullptr;
        for (size_t j = 0; j < all_file_count; j++) {
            upb_StringView cur_name_sv = google_protobuf_FileDescriptorProto_name(all_files[j]);
            if (name == std::string(cur_name_sv.data, cur_name_sv.size)) {
                file_proto = all_files[j];
                break;
            }
        }
        if (!file_proto) continue;

        package_name_ = file_to_package_[name];

        // 1. Generate Main Module (.pm)
        std::string module_content = generate_module(file_proto);
        google_protobuf_compiler_CodeGeneratorResponse_File* file = 
            google_protobuf_compiler_CodeGeneratorResponse_add_file(response, arena_);
        
        std::string p = package_name_;
        std::replace(p.begin(), p.end(), ':', '/');
        size_t double_slash;
        while ((double_slash = p.find("//")) != std::string::npos) {
            p.replace(double_slash, 2, "/");
        }
        
        std::string final_path = p + ".pm";
        google_protobuf_compiler_CodeGeneratorResponse_File_set_name(file, arena_copy(final_path));
        google_protobuf_compiler_CodeGeneratorResponse_File_set_content(file, arena_copy(module_content));

        // 2. Generate Types Module (::Types)
        std::string types_content = generate_types(file_proto);
        google_protobuf_compiler_CodeGeneratorResponse_File* types_file = 
            google_protobuf_compiler_CodeGeneratorResponse_add_file(response, arena_);
        std::string types_path = p + "/Types.pm";
        google_protobuf_compiler_CodeGeneratorResponse_File_set_name(types_file, arena_copy(types_path));
        google_protobuf_compiler_CodeGeneratorResponse_File_set_content(types_file, arena_copy(types_content));
    }

    return response;
}

std::string PerlCodeGenerator::generate_module(const google_protobuf_FileDescriptorProto* file_proto) {
    std::stringstream ss;
    ss << "package " << package_name_ << ";" << std::endl << std::endl;
    ss << "use strict;" << std::endl;
    ss << "use warnings;" << std::endl << std::endl;
    ss << "our $VERSION = '0.01';" << std::endl << std::endl;
    
    ss << "use Protobuf::Message;" << std::endl;
    ss << "use Protobuf::DescriptorPool;" << std::endl;
    ss << "use Protobuf::Internal qw(:all);" << std::endl;
    ss << "use Const::Fast;" << std::endl;
    ss << "use MIME::Base64;" << std::endl << std::endl;

    ss << "BEGIN {" << std::endl;
    
    // Add requires for dependencies INSIDE BEGIN block
    size_t dep_count;
    const upb_StringView* deps = google_protobuf_FileDescriptorProto_dependency(file_proto, &dep_count);
    for (size_t i = 0; i < dep_count; i++) {
        std::string dep(deps[i].data, deps[i].size);
        if (file_to_package_.count(dep)) {
            ss << "    eval { require " << file_to_package_[dep] << " };" << std::endl;
        }
    }

    ss << "    my $descriptor_b64 = <<'EOF';" << std::endl;
    
    size_t size;
    char* buf = google_protobuf_FileDescriptorProto_serialize(file_proto, arena_, &size);
    
    static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (size_t i = 0; i < size; i += 3) {
        uint32_t val = (uint8_t)buf[i] << 16;
        if (i + 1 < size) val |= (uint8_t)buf[i+1] << 8;
        if (i + 2 < size) val |= (uint8_t)buf[i+2];
        
        ss << b64_table[(val >> 18) & 0x3F];
        ss << b64_table[(val >> 12) & 0x3F];
        ss << (i + 1 < size ? b64_table[(val >> 6) & 0x3F] : '=');
        ss << (i + 2 < size ? b64_table[val & 0x3F] : '=');
        
        if ((i / 3 + 1) % 18 == 0) ss << std::endl;
    }
    ss << std::endl << "EOF" << std::endl;
    ss << "    Protobuf::DescriptorPool->generated_pool->add_serialized_file(MIME::Base64::decode_base64($descriptor_b64));" << std::endl;
    ss << "}" << std::endl << std::endl;

    ss << "# Message definitions" << std::endl << std::endl;
    
    size_t message_count;
    const google_protobuf_DescriptorProto* const* messages = 
        google_protobuf_FileDescriptorProto_message_type(file_proto, &message_count);
    
    for (size_t i = 0; i < message_count; i++) {
        const google_protobuf_DescriptorProto* msg = messages[i];
        upb_StringView msg_name_sv = google_protobuf_DescriptorProto_name(msg);
        std::string msg_name(msg_name_sv.data, msg_name_sv.size);
        
        ss << "# === Message: " << package_name_ << "::" << msg_name << " ===" << std::endl;
        ss << "    # Fields for " << msg_name << std::endl;
        
        size_t field_count;
        const google_protobuf_FieldDescriptorProto* const* fields = 
            google_protobuf_DescriptorProto_field(msg, &field_count);
        
        for (size_t j = 0; j < field_count; j++) {
            const google_protobuf_FieldDescriptorProto* f = fields[j];
            upb_StringView f_name_sv = google_protobuf_FieldDescriptorProto_name(f);
            upb_StringView f_type_name_sv = google_protobuf_FieldDescriptorProto_type_name(f);
            int f_type = google_protobuf_FieldDescriptorProto_type(f);
            
            ss << "    # Field: " << std::string(f_name_sv.data, f_name_sv.size);
            ss << " Type: " << f_type << " (" << std::string(f_type_name_sv.data, f_type_name_sv.size) << ")" << std::endl;
        }
        ss << std::endl;

        size_t enum_count;
        const google_protobuf_EnumDescriptorProto* const* enums = 
            google_protobuf_DescriptorProto_enum_type(msg, &enum_count);
        for (size_t j = 0; j < enum_count; j++) {
            const google_protobuf_EnumDescriptorProto* e = enums[j];
            upb_StringView e_name_sv = google_protobuf_EnumDescriptorProto_name(e);
            std::string e_name(e_name_sv.data, e_name_sv.size);
            
            ss << "# Enum: " << msg_name << "::" << e_name << std::endl;
            size_t val_count;
            const google_protobuf_EnumValueDescriptorProto* const* values = 
                google_protobuf_EnumDescriptorProto_value(e, &val_count);
            for (size_t k = 0; k < val_count; k++) {
                const google_protobuf_EnumValueDescriptorProto* ev = values[k];
                upb_StringView ev_name_sv = google_protobuf_EnumValueDescriptorProto_name(ev);
                int ev_num = google_protobuf_EnumValueDescriptorProto_number(ev);
                ss << "const my $" << msg_name << "_" << std::string(ev_name_sv.data, ev_name_sv.size) << " => " << ev_num << ";" << std::endl;
            }
            ss << std::endl;
        }
    }

    // Service definitions
    size_t service_count;
    const google_protobuf_ServiceDescriptorProto* const* services = 
        google_protobuf_FileDescriptorProto_service(file_proto, &service_count);
    
    upb_StringView pkg_sv = google_protobuf_FileDescriptorProto_package(file_proto);
    std::string proto_pkg(pkg_sv.data, pkg_sv.size);
    std::string default_host = get_default_host(proto_pkg);

    for (size_t i = 0; i < service_count; i++) {
        const google_protobuf_ServiceDescriptorProto* srv = services[i];
        upb_StringView srv_name_sv = google_protobuf_ServiceDescriptorProto_name(srv);
        std::string srv_name(srv_name_sv.data, srv_name_sv.size);
        
        ss << "# === Service Client: " << package_name_ << "::" << srv_name << "Client ===" << std::endl;
        ss << "package " << package_name_ << "::" << srv_name << "Client;" << std::endl << std::endl;
        ss << "use Moo;" << std::endl;
        ss << "use Google::Auth;" << std::endl;
        ss << "use Google::gRPC::Client;" << std::endl << std::endl;

        ss << "has credentials => ( is => 'ro', default => sub { Google::Auth->default() } );" << std::endl;
        ss << "has target      => ( is => 'ro', default => '" << default_host << "' );" << std::endl << std::endl;

        ss << "has _grpc_client => (" << std::endl;
        ss << "    is => 'ro'," << std::endl;
        ss << "    lazy => 1," << std::endl;
        ss << "    builder => sub {" << std::endl;
        ss << "        my $self = shift;" << std::endl;
        ss << "        return Google::gRPC::Client->new(" << std::endl;
        ss << "            target     => $self->target," << std::endl;
        ss << "            auth_token => $self->credentials->get_token()," << std::endl;
        ss << "        );" << std::endl;
        ss << "    }" << std::endl;
        ss << ");" << std::endl << std::endl;
        
        size_t method_count;
        const google_protobuf_MethodDescriptorProto* const* methods = 
            google_protobuf_ServiceDescriptorProto_method(srv, &method_count);
        for (size_t j = 0; j < method_count; j++) {
            const google_protobuf_MethodDescriptorProto* m = methods[j];
            upb_StringView m_name_sv = google_protobuf_MethodDescriptorProto_name(m);
            std::string m_name(m_name_sv.data, m_name_sv.size);

            upb_StringView in_type_sv = google_protobuf_MethodDescriptorProto_input_type(m);
            std::string in_type(in_type_sv.data, in_type_sv.size);
            std::string req_class = resolve_perl_class(in_type);

            upb_StringView out_type_sv = google_protobuf_MethodDescriptorProto_output_type(m);
            std::string out_type(out_type_sv.data, out_type_sv.size);
            std::string res_class = resolve_perl_class(out_type);

            std::string snake_method = to_snake_case(m_name);

            ss << "sub " << snake_method << " {" << std::endl;
            ss << "    my ($self, $args) = @_;" << std::endl;
            ss << "    my $req = ref($args) eq 'HASH'" << std::endl;
            ss << "        ? " << req_class << "->new($args)" << std::endl;
            ss << "        : $args;" << std::endl;
            ss << "    return $self->_grpc_client->call({" << std::endl;
            ss << "        service        => '" << proto_pkg << "." << srv_name << "'," << std::endl;
            ss << "        method         => '" << m_name << "'," << std::endl;
            ss << "        request        => $req," << std::endl;
            ss << "        response_class => '" << res_class << "'," << std::endl;
            ss << "    });" << std::endl;
            ss << "}" << std::endl << std::endl;
        }
    }

    ss << "1;" << std::endl;
    return ss.str();
}

std::string PerlCodeGenerator::generate_types(const google_protobuf_FileDescriptorProto* file_proto) {
    std::stringstream ss;
    ss << "package " << package_name_ << "::Types;" << std::endl << std::endl;
    ss << "use strict;" << std::endl;
    ss << "use warnings;" << std::endl << std::endl;
    ss << "use Type::Library -base;" << std::endl;
    ss << "use Type::Utils -all;" << std::endl;
    ss << "use Types::Standard -types;" << std::endl << std::endl;

    size_t enum_count;
    const google_protobuf_EnumDescriptorProto* const* enums =
        google_protobuf_FileDescriptorProto_enum_type(file_proto, &enum_count);
    for (size_t j = 0; j < enum_count; ++j) {
        print_enum_types(enums[j], package_name_, ss);
    }

    size_t message_count;
    const google_protobuf_DescriptorProto* const* messages =
        google_protobuf_FileDescriptorProto_message_type(file_proto, &message_count);
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

    print_message_types(msg_proto, current_package, ss);

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

void PerlCodeGenerator::print_message_types(const google_protobuf_DescriptorProto* msg_proto, const std::string& current_package, std::stringstream& ss) {
    upb_StringView msg_name_sv = google_protobuf_DescriptorProto_name(msg_proto);
    std::string msg_name(msg_name_sv.data, msg_name_sv.size);
    std::string full_perl_class = current_package + "::" + msg_name;

    ss << "declare '" << msg_name << "'," << std::endl;
    ss << "    as InstanceOf['" << full_perl_class << "'];" << std::endl << std::endl;

    ss << "coerce '" << msg_name << "'," << std::endl;
    ss << "    from HashRef, via { '" << full_perl_class << "'->new($_) };" << std::endl << std::endl;

    // Add Repeated and Map variants
    ss << "declare 'Repeated" << msg_name << "'," << std::endl;
    ss << "    as ArrayRef[" << msg_name << "()];" << std::endl << std::endl;

    ss << "coerce 'Repeated" << msg_name << "'," << std::endl;
    ss << "    from ArrayRef[HashRef], via { [ map { '" << full_perl_class << "'->new($_) } @$_ ] };" << std::endl << std::endl;

    ss << "declare 'MapString" << msg_name << "'," << std::endl;
    ss << "    as HashRef[" << msg_name << "()];" << std::endl << std::endl;
}

void PerlCodeGenerator::print_enum_types(const google_protobuf_EnumDescriptorProto* enum_proto, const std::string& current_package, std::stringstream& ss) {
    upb_StringView enum_name_sv = google_protobuf_EnumDescriptorProto_name(enum_proto);
    std::string enum_name(enum_name_sv.data, enum_name_sv.size);

    ss << "declare '" << enum_name << "'," << std::endl;
    ss << "    as (Int | Str);" << std::endl << std::endl;
}

// -- main --

int main(int argc, char** argv) {
    upb_Arena* arena = upb_Arena_New();
    
    std::string input;
    char buffer[4096];
    while (std::cin.read(buffer, sizeof(buffer))) {
        input.append(buffer, std::cin.gcount());
    }
    input.append(buffer, std::cin.gcount());

    google_protobuf_compiler_CodeGeneratorRequest* request = 
        google_protobuf_compiler_CodeGeneratorRequest_parse(input.data(), input.length(), arena);
    
    if (!request) {
        std::cerr << "Failed to parse CodeGeneratorRequest" << std::endl;
        upb_Arena_Free(arena);
        return 1;
    }

    PerlCodeGenerator generator(request, arena);
    google_protobuf_compiler_CodeGeneratorResponse* response = generator.generate();

    size_t size;
    char* buf = google_protobuf_compiler_CodeGeneratorResponse_serialize(response, arena, &size);
    std::cout.write(buf, size);

    upb_Arena_Free(arena);
    return 0;
}
