#include "interface.h"
#include <string>
#include <vector>
#include <cstdint>
#include <simdutf.h>

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t transcode_checksum(const std::u16string& input) {
  // Maximum expansion: each UTF-16 code unit can expand to 3 UTF-8 bytes.
  // (Surrogate pairs are two code units -> 4 bytes, so factor 2 < 3)
  size_t max_len = input.size() * 3 + 7; // small extra for safety
  std::vector<char> buffer(max_len);
  size_t written = simdutf::convert_utf16le_to_utf8(input.data(), input.size(), buffer.data());
  // Input is assumed valid; written > 0 unless empty.
  return checksum_bytes(buffer.data(), written);
}