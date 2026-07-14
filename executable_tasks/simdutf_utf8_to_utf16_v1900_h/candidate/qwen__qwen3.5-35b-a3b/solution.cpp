#pragma once

#include <cstdint>
#include <string>
#include <simdutf.h>
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
  std::vector<char16_t> out(input.size());
  size_t code_units = simdutf::utf8_to_utf16(input.data(), input.size(), out.data());
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < code_units; ++i) {
    hash ^= static_cast<uint64_t>(out[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}