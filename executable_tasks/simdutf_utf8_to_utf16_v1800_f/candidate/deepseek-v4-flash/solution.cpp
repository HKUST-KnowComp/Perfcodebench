#include "interface.h"
#include <vector>
#include <simdutf.h>

uint64_t transcode_checksum(const std::string& input) {
  size_t utf16_len = simdutf::utf16_length_from_utf8(input.data(), input.size());
  std::vector<char16_t> out(utf16_len);
  size_t result = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < result; ++i) {
    hash ^= static_cast<uint64_t>(out[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}