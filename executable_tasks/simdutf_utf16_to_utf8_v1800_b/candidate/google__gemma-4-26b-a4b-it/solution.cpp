#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a hash implementation to match the original checksum logic
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
  // We use the version that returns a std::string for convenience, 
  // or manually manage a buffer if performance profiling suggests string allocation is a bottleneck.
  // Given the task, simdutf's optimized transcoding is the primary win.
  
  std::string utf8_output;
  // simdutf provides optimized conversion. 
  // Note: simdutf expects char16_t, std::u16string uses char16_t.
  // We use the error-handling version that assumes valid input as per task description.
  
  size_t utf8_len = simdutf::utf16_length_in_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size());
  utf8_output.resize(utf8_len);
  
  simdutf::convert_utf16_to_utf8(reinterpret_cast<const char16_t*>(input.data()), 
                                 input.size(), 
                                 reinterpret_cast<char*>(utf8_output.data()));

  return checksum_bytes(utf8_output);
}