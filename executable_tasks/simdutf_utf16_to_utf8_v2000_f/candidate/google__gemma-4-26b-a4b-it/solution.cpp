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

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return checksum_bytes("");
  }

  // simdutf_convert_utf16_to_utf8 requires a destination buffer.
  // We first determine the required size or use a reasonable estimate.
  // To ensure correctness and performance, we use the simdutf API to transcode.
  
  // simdutf::convert_utf16_to_utf8 returns a std::string containing the UTF-8 result.
  // Note: The library handles surrogate pairs and validation.
  
  std::string utf8_result;
  try {
    // We use the simdutf library's optimized conversion.
    // Since the task specifies valid UTF-16LE, we can use the fast path.
    // simdutf::convert_utf16_to_utf8 is the standard high-level interface.
    
    // Note: simdutf::convert_utf16_to_utf8 is available in the simdutf header.
    // We use the version that takes a pointer to avoid unnecessary copies if possible,
    // but the standard string return is safest for the interface.
    
    // Pre-calculating size or using a large enough buffer to avoid reallocations.
    // A safe upper bound for UTF-8 is 3 bytes per UTF-16 code unit (for BMP) 
    // or 2 bytes per UTF-16 code unit (for surrogates).
    // However, simdutf handles the allocation internally.
    
    utf8_result = simdutf::convert_utf16_to_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size());
  } catch (...) {
    // Fallback or error handling if input is invalid (though task says valid)
    return 0;
  }

  return checksum_bytes(utf8_result);
}
