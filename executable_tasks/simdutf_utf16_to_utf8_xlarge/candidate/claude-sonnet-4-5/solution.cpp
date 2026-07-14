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

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return checksum_bytes("", 0);
  }

  // Calculate exact output size needed
  size_t utf8_len = simdutf::utf8_length_from_utf16le(
      reinterpret_cast<const char16_t*>(input.data()), input.size());

  // Allocate buffer with exact size
  std::vector<char> utf8_buffer(utf8_len);

  // Perform SIMD-optimized conversion
  simdutf::convert_utf16le_to_utf8(
      reinterpret_cast<const char16_t*>(input.data()), input.size(),
      utf8_buffer.data());

  return checksum_bytes(utf8_buffer.data(), utf8_len);
}
