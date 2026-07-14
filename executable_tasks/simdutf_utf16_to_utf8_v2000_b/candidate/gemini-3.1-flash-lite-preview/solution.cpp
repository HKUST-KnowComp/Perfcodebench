#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(ptr[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  // UTF-16 to UTF-8 max expansion is 3x (or 4x for surrogate pairs, but 3x is safe for standard UTF-16)
  // simdutf::utf8_length provides the exact required size.
  size_t utf8_len = simdutf::utf8_length_from_utf16(input.data(), input.size());
  std::vector<char> buffer(utf8_len);
  
  size_t written = simdutf::convert_utf16_to_utf8(input.data(), input.size(), buffer.data());
  
  return checksum_bytes(buffer.data(), written);
}