#include "interface.h"
#include "simdutf.h"
#include <string"
#include <vector>

namespace {

// FNV-1a hash implementation as per the baseline
uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
  if (input.empty()) {
    return checksum_bytes("");
  }

  // Use simdutf for high-performance SIMD-accelerated base64 decoding
  // simdutf::base64::unpack returns a std::string containing the decoded bytes
  // It handles padding and validation efficiently.
  try {
    std::string decoded = simdutf::base64::unpack(input);
    return checksum_bytes(decoded);
  } catch (...) {
    // Fallback or error handling if input is malformed, though task implies valid base64
    return 0;
  }
}
