#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a hash implementation as per the original logic
inline uint64_t checksum_bytes(const std::string& data) {
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

  // simdutf::convert_utf16_to_utf8 is highly optimized using SIMD.
  // It handles surrogate pairs and valid UTF-16LE input.
  // We use the version that returns a std::string for convenience,
  // though for absolute peak performance one might use a pre-allocated buffer.
  std::string utf8_output;
  try {
    // simdutf::convert_utf16_to_utf8 expects a pointer and size.
    // Note: simdutf handles the conversion efficiently.
    utf8_output = simdutf::convert_utf16_to_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size());
  } catch (...) {
    // Fallback or error handling if input is invalid, though task implies valid UTF-16LE.
    return 0;
  }

  return checksum_bytes(utf8_output);
}
