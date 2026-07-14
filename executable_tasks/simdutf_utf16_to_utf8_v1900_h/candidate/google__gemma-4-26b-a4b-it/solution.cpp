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

  // simdutf::convert_utf16_to_utf8 requires a destination buffer.
  // We first calculate the maximum possible size for UTF-8 to avoid reallocations.
  // A safe upper bound for UTF-16 to UTF-8 is input.size() * 3.
  // However, simdutf provides a way to get the required size or we can use a large enough buffer.
  
  size_t utf8_len = simdutf::utf16_to_utf8_length(reinterpret_cast<const char16_t*>(input.data()), input.size());
  std::string out(utf8_len, '\0');

  // Perform the SIMD-accelerated transcoding
  size_t converted = simdutf::convert_utf16_to_utf8(
      reinterpret_cast<const char16_t*>(input.data()), 
      input.size(), 
      reinterpret_cast<char*>(out.data())
  );

  // Ensure the string size matches the actual converted length
  out.resize(converted);

  return checksum_bytes(out);
}
