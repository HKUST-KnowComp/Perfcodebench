#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <simdutf.h>

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }

  size_t max_utf8_size = input.size() * 4;
  std::vector<char> buffer(max_utf8_size);
  size_t utf8_len = simdutf::utf16_to_utf8(input.data(), input.size(), buffer.data());

  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < utf8_len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buffer[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}