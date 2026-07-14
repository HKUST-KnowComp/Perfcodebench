#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <simdutf.h>

uint64_t transcode_checksum(const std::string& input) {
  size_t max_output_size = input.size() * 2;
  std::vector<char16_t> output(max_output_size);
  size_t output_size = simdutf::utf8_to_utf16(input.data(), input.size(), output.data());
  
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < output_size; ++i) {
    hash ^= static_cast<uint64_t>(output[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}