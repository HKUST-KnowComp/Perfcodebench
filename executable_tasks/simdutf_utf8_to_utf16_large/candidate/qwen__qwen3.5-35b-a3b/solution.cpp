#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <simdutf.h>

namespace {

uint64_t checksum_utf16(const char16_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  if (input.empty()) return 0;
  std::vector<char16_t> out(input.size() * 2);
  size_t len = simdutf::utf8_to_utf16(input.data(), input.size(), out.data());
  return checksum_utf16(out.data(), len);
}