// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_PROTOBUF_COMPILER_PERL_HELPERS_H__
#define THIRD_PARTY_PROTOBUF_COMPILER_PERL_HELPERS_H__

#include <string>
#include <string_view>
#include <vector>

namespace google {
namespace protobuf {
namespace compiler {
namespace perl {

// Custom string helpers to avoid Abseil library dependencies
bool ConsumePrefix(std::string_view* s, std::string_view prefix);
std::vector<std::string_view> SplitString(std::string_view s, char delim);

// Converts a string to CamelCase, with custom mappings for known words.
std::string ToCamelCase(std::string_view s);

// Converts a CamelCase string to snake_case.
std::string ToSnakeCase(std::string_view s);

// Extracts the default host for standard Google Cloud APIs based on the package prefix.
std::string GetDefaultHost(std::string_view proto_pkg);

// Capitalizes each segment of a package name (e.g. foo.bar -> Foo.Bar), with custom mappings.
std::string CapitalizePackage(std::string_view s);

}  // namespace perl
}  // namespace compiler
}  // namespace protobuf
}  // namespace google

#endif  // THIRD_PARTY_PROTOBUF_COMPILER_PERL_HELPERS_H__
