#include "interface.h"
#include "simdutf.h"
#include <string"
#include <vector>

namespace {

// FNV-1a hash implementation to match the baseline checksum logic
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
  // Note: simdutf is highly optimized for various architectures (AVX2, NEON, etc.)
  std::string decoded;
  try {
    // We use the padded version as the baseline handles '=' padding.
    // simdutf's decode is much faster than manual bit-shifting.
    simdutf::base64::unpack_padded(input, decoded);
  } catch (...) {
    // Fallback or error handling if input is malformed, 
    // though the task implies valid base64.
    return 0;
  }

  return checksum_bytes(decoded);
}
