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

  // simdutf::convert_utf16_to_utf8 is highly optimized using SIMD.
  // We use a temporary string to hold the result.
  // Note: simdutf expects a pointer to char16_t.
  
  // First, determine the required size or use a reasonable buffer.
  // For performance in a single-pass, we can use the library's ability to transcode.
  // Since we need to return a checksum of the exact bytes, we must perform the full transcode.
  
  size_t utf8_len = simdutf::utf16_length_in_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size());
  std::string out;
  out.resize(utf8_len);

  size_t converted = simdutf::convert_utf16_to_utf8(
      reinterpret_cast<const char16_t*>(input.data()), 
      input.size(), 
      reinterpret_cast<char*>(out.data())
  );

  // In case of unexpected length (though length calculation should be exact)
  if (converted < utf8_len) {
      out.resize(converted);
  }

  return checksum_bytes(out);
}
