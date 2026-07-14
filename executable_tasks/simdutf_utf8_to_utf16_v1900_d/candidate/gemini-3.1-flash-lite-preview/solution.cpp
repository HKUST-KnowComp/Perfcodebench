#include "interface.h"
#include "simdutf.h"
#include <vector>

/**
 * Optimized implementation using simdutf for high-performance transcoding.
 * We use the library's ability to calculate the required output length first,
 * then perform the conversion in a single pass.
 */
uint64_t transcode_checksum(const std::string& input) {
  simdutf::implementation* impl = simdutf::get_available_implementations().detect_best();
  
  size_t utf16_len = impl->utf16_length_from_utf8(input.data(), input.size());
  std::vector<char16_t> out(utf16_len);
  
  size_t actual_len = impl->convert_utf8_to_utf16(input.data(), input.size(), out.data());
  
  // FNV-1a checksum calculation
  uint64_t hash = 1469598103934665603ULL;
  const char16_t* data = out.data();
  for (size_t i = 0; i < actual_len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}