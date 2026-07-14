#include "interface.h"
#include "simdutf.h"
#include <vector>

/**
 * Optimized implementation using simdutf library.
 * simdutf provides hardware-accelerated transcoding which is significantly
 * faster than manual scalar loops.
 */
uint64_t transcode_checksum(const std::string& input) {
  simdutf::implementation* impl = simdutf::get_available_implementations().detect_best();
  
  // Calculate required buffer size
  size_t utf16_len = impl->utf16_length_from_utf8(input.data(), input.size());
  std::vector<char16_t> out(utf16_len);
  
  // Transcode
  size_t written = impl->convert_utf8_to_utf16(input.data(), input.size(), out.data());
  
  // Compute checksum
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < written; ++i) {
    hash ^= static_cast<uint64_t>(out[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}