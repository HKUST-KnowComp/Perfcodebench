#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::string& input) {
  // Use simdutf to get the required output length
  size_t utf16_len = simdutf::utf16_length_from_utf8(input.data(), input.size());
  
  // Allocate output buffer
  std::vector<char16_t> out(utf16_len);
  
  // Perform SIMD-accelerated transcoding
  simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
  
  // Compute FNV-1a checksum with unrolling
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  const char16_t* data = out.data();
  size_t len = out.size();
  size_t i = 0;
  
  // Process 8 elements at a time
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
  
  // Handle remaining elements
  for (; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
  }
  
  return hash;
}