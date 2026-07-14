#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

// FNV-1a 64-bit hash implementation
inline uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(p[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  simdutf::implementation& impl = *simdutf::get_available_implementations().detect_best_supported();
  
  // Calculate required buffer size
  size_t max_len = impl.utf8_length_from_utf16(input.size(), reinterpret_cast<const char16_t*>(input.data()));
  std::vector<char> buffer(max_len);
  
  // Perform transcoding
  size_t actual_len = impl.convert_utf16_to_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size(), buffer.data());
  
  return checksum_bytes(buffer.data(), actual_len);
}