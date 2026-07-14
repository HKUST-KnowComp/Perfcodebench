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
  std::vector<char16_t> out;
  out.reserve(input.size() * 2);
  const char* input_data = input.data();
  size_t input_len = input.size();
  size_t output_len = simdutf::utf8_to_utf16le(input_data, input_len, out.data());
  out.resize(output_len);
  return checksum_utf16(out);
}