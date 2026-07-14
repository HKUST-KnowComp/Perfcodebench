#include "interface.h"

#include <string>
#include <simdutf.h>

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return checksum_bytes("");
  }
  
  // Calculate required output size using simdutf
  std::size_t utf8_size = simdutf::convert_utf16_to_utf8(input.data(), input.size(), nullptr);
  
  // Allocate output buffer
  std::string out;
  out.resize(utf8_size);
  
  // Perform SIMD-accelerated conversion
  std::size_t bytes_written = simdutf::convert_utf16_to_utf8(input.data(), input.size(), out.data());
  
  return checksum_bytes(out);
}