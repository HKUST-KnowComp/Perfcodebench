#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

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
    return 1469598103934665603ULL;
  }
  
  // Calculate required UTF-8 length
  size_t utf8_len = simdutf::utf8_length_from_utf32(
      reinterpret_cast<const char32_t*>(input.data()), input.size());
  
  // Allocate buffer for UTF-8 output
  std::vector<char> utf8_buffer(utf8_len);
  
  // Convert UTF-32 to UTF-8 using simdutf
  size_t written = simdutf::convert_utf32_to_utf8(
      reinterpret_cast<const char32_t*>(input.data()), input.size(),
      utf8_buffer.data());
  
  return checksum_bytes(utf8_buffer.data(), written);
}