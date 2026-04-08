#ifndef PERL_STRING_UTILS_H
#define PERL_STRING_UTILS_H

#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace PerlGenUtils {

inline std::string capitalize(std::string s) {
    if (!s.empty()) {
        s[0] = std::toupper(s[0]);
    }
    return s;
}

inline std::string to_camel_case(const std::string& s) {
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

inline std::string base64_encode(const std::string& in) {
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

inline std::string get_module_base_name(const std::string& package) {
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

inline std::string get_perl_package_name(const std::string& proto_file_path, const std::string& package) {
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

inline std::string proto_type_to_perl_module(const std::string& type_name) {
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

} // namespace PerlGenUtils

#endif // PERL_STRING_UTILS_H
