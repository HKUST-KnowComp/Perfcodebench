#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a 64-bit hash implementation
uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data.data());
  size_t len = data.size();
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(ptr[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return checksum_bytes("");
  }

  // Use simdutf for high-performance transcoding.
  // simdutf::convert_utf16_to_utf8 returns a std::string containing the UTF-8 result.
  // It uses SIMD instructions (AVX2, SSE4.2, or NEON) internally.
  std::string out = simdutf::convert_utf16_to_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size());

  return checksum_bytes(out);
}
