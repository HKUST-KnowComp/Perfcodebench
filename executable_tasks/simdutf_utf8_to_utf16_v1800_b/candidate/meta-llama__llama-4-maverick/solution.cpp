#include "interface.h"
#include "simdutf.h"

uint64_t transcode_checksum(const std::string& input) {
  size_t utf16_len = simdutf::utf16_length_from_utf8(input.data(), input.size());
  std::vector<char16_t> utf16(utf16_len);
  simdutf::convert_utf8_to_utf16le(input.data(), input.size(), utf16.data());
  uint64_t hash = 1469598103934665603ULL;
  for (char16_t ch : utf16) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}