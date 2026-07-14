#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
  simdutf::implementation& impl = *simdutf::get_available_implementations().detect_best();
  
  // Calculate maximum possible size for UTF-8 output
  size_t max_len = impl.utf8_length_from_utf32(input.data(), input.size());
  std::vector<char> buffer(max_len);
  
  // Transcode using SIMD
  size_t actual_len = impl.convert_utf32_to_utf8(input.data(), input.size(), buffer.data());
  
  // FNV-1a checksum calculation
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* data = reinterpret_cast<const uint8_t*>(buffer.data());
  for (size_t i = 0; i < actual_len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}