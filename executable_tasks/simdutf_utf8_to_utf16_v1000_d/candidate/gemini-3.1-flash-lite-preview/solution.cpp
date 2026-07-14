#include "interface.h"
#include "simdutf.h"
#include <vector>

namespace {

// FNV-1a hash implementation for the UTF-16 sequence
inline uint64_t checksum_utf16(const char16_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  simdutf::implementation& impl = *simdutf::get_available_implementations().detect_best_supported();
  
  // Calculate maximum possible length for UTF-16 output
  size_t max_len = impl.utf16_length_from_utf8(input.data(), input.size());
  std::vector<char16_t> out(max_len);
  
  // Transcode using SIMD
  size_t actual_len = impl.convert_utf8_to_utf16(input.data(), input.size(), out.data());
  
  return checksum_utf16(out.data(), actual_len);
}