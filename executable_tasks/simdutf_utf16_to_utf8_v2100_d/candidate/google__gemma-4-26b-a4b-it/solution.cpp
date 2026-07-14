#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a hash implementation as per the original baseline
uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return checksum_bytes("");
  }

  // simdutf::convert_utf16_to_utf8 is highly optimized using SIMD instructions.
  // It handles surrogate pairs and produces a valid UTF-8 string.
  // We use the string-based overload for convenience, which is efficient.
  try {
    std::string utf8_out = simdutf::convert_utf16_to_utf8(input);
    return checksum_bytes(utf8_out);
  } catch (...) {
    // Fallback or error handling if input is invalid, though task implies valid UTF-16LE.
    // In a production environment, we would handle invalid sequences specifically.
    return 0;
  }
}
