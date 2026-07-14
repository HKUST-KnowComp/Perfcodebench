#include "interface.h"

#include <simdutf.h>
#include <string>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }
  
  // Calculate required UTF-8 buffer size
  size_t utf8_len = simdutf::utf8_length_from_utf16le(
      reinterpret_cast<const char16_t*>(input.data()), input.size());
  
  // Allocate buffer for UTF-8 output
  std::string out;
  out.resize(utf8_len);
  
  // Perform SIMD-accelerated transcoding
  simdutf::convert_utf16le_to_utf8(
      reinterpret_cast<const char16_t*>(input.data()), input.size(),
      out.data());
  
  // Compute FNV-1a checksum with unrolling
  const unsigned char* data = reinterpret_cast<const unsigned char*>(out.data());
  size_t len = out.size();
  uint64_t hash = 1469598103934665603ULL;
  constexpr uint64_t prime = 1099511628211ULL;
  
  // Process 8 bytes at a time
  size_t i = 0;
  size_t end8 = len & ~size_t(7);
  for (; i < end8; i += 8) {
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
  
  // Handle remaining bytes
  for (; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
  }
  
  return hash;
}