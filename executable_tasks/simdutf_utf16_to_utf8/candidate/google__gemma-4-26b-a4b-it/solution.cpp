#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a hash implementation as per the baseline requirement
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

  // simdutf::convert_utf16_to_utf8 is highly optimized for SIMD architectures.
  // It handles surrogate pairs and produces a valid UTF-8 string.
  // We use the version that returns a std::string for convenience, 
  // or we can use the buffer-based one if we wanted to avoid one allocation.
  // Given the interface, we'll use the standard conversion.
  
  std::string utf8_out;
  // simdutf::convert_utf16_to_utf8 expects a pointer to char16_t
  // and handles the conversion efficiently.
  // Note: simdutf usually expects UTF-16 (little endian on most systems).
  // The task specifies UTF-16LE.
  
  try {
    // We use the simdutf API to perform the conversion.
    // The library is designed to be much faster than manual loops.
    utf8_out = simdutf::convert_utf16_to_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size());
  } catch (...) {
    // Fallback or error handling if necessary, though task implies valid input
    return 0;
  }

  return checksum_bytes(utf8_out);
}
