#include "interface.h"
#include <string>
#include <simdutf.h>

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) return 1469598103934665603ULL;

  // Calculate required UTF-8 buffer size
  const size_t max_utf8_size = input.size() * 3;
  std::string out;
  out.resize(max_utf8_size);

  // Use simdutf for optimized transcoding
  const size_t utf8_len = simdutf::convert_utf16_to_utf8(input.data(), input.size(), out.data());
  out.resize(utf8_len);

  // Compute FNV-1a checksum
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : out) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}