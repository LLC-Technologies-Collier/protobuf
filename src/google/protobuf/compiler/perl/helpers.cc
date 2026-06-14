// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "helpers.h"
#include <utility>
#include <vector>
#include <cctype>

namespace google {
namespace protobuf {
namespace compiler {
namespace perl {

bool ConsumePrefix(std::string_view* s, std::string_view prefix) {
  if (s->starts_with(prefix)) {
    s->remove_prefix(prefix.length());
    return true;
  }
  return false;
}

std::vector<std::string_view> SplitString(std::string_view s, char delim) {
  std::vector<std::string_view> res;
  size_t start = 0;
  while (start < s.length()) {
    size_t end = s.find(delim, start);
    if (end == std::string_view::npos) {
      res.push_back(s.substr(start));
      break;
    }
    res.push_back(s.substr(start, end - start));
    start = end + 1;
  }
  return res;
}

std::string ToCamelCase(std::string_view s) {
  static constexpr std::pair<std::string_view, std::string_view>
      kCustomMappings[] = {{"bigquery", "BigQuery"}, {"pubsub", "PubSub"}};

  std::string res;
  for (std::string_view segment : SplitString(s, '_')) {
    if (segment.empty()) continue;

    bool found_custom = false;
    for (const auto& [key, val] : kCustomMappings) {
      if (segment == key) {
        res += val;
        found_custom = true;
        break;
      }
    }
    if (!found_custom) {
      res += (char)toupper(segment[0]);
      res += segment.substr(1);
    }
  }
  return res;
}

std::string ToSnakeCase(std::string_view s) {
  std::string res;
  res.reserve(s.length() + 8); // Pre-allocate memory upfront!
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (isupper(c)) {
      if (i > 0 && s[i - 1] != '_') {
        res += '_';
      }
      res += (char)tolower(c);
    } else {
      res += c;
    }
  }
  return res;
}

std::string GetDefaultHost(std::string_view proto_pkg) {
  std::string_view sub = proto_pkg;
  if (ConsumePrefix(&sub, "google.cloud.") ||
      ConsumePrefix(&sub, "google.")) {
    size_t end = sub.find('.');
    if (end != std::string_view::npos) {
      return std::string(sub.substr(0, end)) + ".googleapis.com:443";
    }
  }
  return "localhost:443";
}

std::string CapitalizePackage(std::string_view s) {
  static constexpr std::pair<std::string_view, std::string_view>
      kCustomMappings[] = {{"bigquery", "BigQuery"}, {"pubsub", "PubSub"}};

  std::string res;
  bool first = true;
  for (std::string_view segment : SplitString(s, '.')) {
    if (!first) res += "::";
    if (!segment.empty()) {
      bool found_custom = false;
      for (const auto& [key, val] : kCustomMappings) {
        if (segment == key) {
          res += val;
          found_custom = true;
          break;
        }
      }
      if (!found_custom) {
        res += (char)toupper(segment[0]);
        res += segment.substr(1);
      }
    }
    first = false;
  }
  return res;
}

}  // namespace perl
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
