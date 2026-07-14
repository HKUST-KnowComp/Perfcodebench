#include "interface.h"

#include <simdutf.h>
#include <vector>
#include <cstdint>

namespace {

uint64_t checksum_utf16(const char16_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  constexpr uint64_t prime = 1099511628211ULL;
  
  size_t i = 0;
  // Unroll by 8 for better ILP
  for (; i + 8 <= len; i += 8) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 1]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 2]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 3]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 4]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 5]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 6]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 7]);
    hash *= prime;
  }
  // Handle remainder
  for (; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }
  
  // Use simdutf to get the required output length
  size_t utf16_len = simdutf::utf16_length_from_utf8(input.data(), input.size());
  
  // Allocate output buffer
  std::vector<char16_t> out(utf16_len);
  
  // Perform SIMD-accelerated transcoding
  size_t written = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
  
  return checksum_utf16(out.data(), written);
}