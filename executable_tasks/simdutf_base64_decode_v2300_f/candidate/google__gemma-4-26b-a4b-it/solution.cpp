#include "interface.h"
#include "simdutf.h"
#include <string>
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

  // simdutf::base64::unpack_padded handles standard base64 with padding.
  // It returns a std::string containing the decoded bytes.
  // This uses SIMD instructions (AVX2/SSE/NEON) internally for maximum performance.
  std::string decoded;
  try {
    // We use the unpack_padded variant which is standard for base64 strings with '='.
    // If the input might not have padding, simdutf::base64::unpack_unpadded could be used,
    // but the baseline logic specifically handles '=' as padding.
    simdutf::base64::unpack_padded(input, decoded);
  } catch (...) {
    // In case of malformed input, fallback to empty or handle error.
    // The baseline didn't specify error handling, so we return checksum of empty.
    return checksum_bytes("");
  }

  return checksum_bytes(decoded);
}
