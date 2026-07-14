#include "interface.h"
#include "simdutf.h"
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
  // simdutf::utf8_to_utf16_length provides the exact buffer size needed
  size_t utf16_len = simdutf::utf8_to_utf16_length(input.data(), input.size());
  std::vector<char16_t> out(utf16_len);

  // Perform the transcoding using simdutf's optimized implementation
  size_t actual_len = simdutf::convert_utf8_to_utf16(input.data(), input.size(), out.data());

  // Compute the checksum over the resulting UTF-16 sequence
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < actual_len; ++i) {
    hash ^= static_cast<uint64_t>(out[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}