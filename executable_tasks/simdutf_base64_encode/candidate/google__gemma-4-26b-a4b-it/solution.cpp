#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a 64-bit hash implementation
uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t encode_checksum(const std::string& input) {
  if (input.empty()) {
    return checksum_bytes("");
  }

  // Use simdutf for high-performance SIMD-accelerated base64 encoding
  // simdutf::base64::encode returns a std::string
  std::string encoded = simdutf::base64::encode(reinterpret_cast<const uint8_t*>(input.data()), input.size());
  
  return checksum_bytes(encoded);
}
