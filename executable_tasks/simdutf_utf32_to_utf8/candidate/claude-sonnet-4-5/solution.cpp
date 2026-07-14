#include "interface.h"

#include <simdutf.h>
#include <string>
#include <vector>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
  if (input.empty()) {
    return checksum_bytes("", 0);
  }

  // Allocate worst-case output buffer (4 bytes per UTF-32 code point)
  size_t max_utf8_len = input.size() * 4;
  std::vector<char> utf8_buffer(max_utf8_len);

  // Use simdutf for optimized SIMD transcoding
  size_t actual_len = simdutf::convert_utf32_to_utf8(
      reinterpret_cast<const char32_t*>(input.data()),
      input.size(),
      utf8_buffer.data());

  return checksum_bytes(utf8_buffer.data(), actual_len);
}
