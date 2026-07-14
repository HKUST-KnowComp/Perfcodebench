#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <simdutf.h>

namespace {

uint64_t checksum_utf16(const std::vector<char16_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (char16_t ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  // Calculate required output size using simdutf
  size_t utf16_size = simdutf::utf8_to_utf16::valid(input.data(), input.size());
  
  std::vector<char16_t> out;
  out.reserve(utf16_size);
  
  // Convert UTF-8 to UTF-16LE using SIMD-accelerated function
  size_t written = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
  
  return checksum_utf16(out);
}