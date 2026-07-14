#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

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

  // simdutf::convert_utf16to8 expects a pointer to char16_t and size in bytes.
  // We use a std::string to hold the result for the checksum function.
  // Note: simdutf's conversion is extremely fast and handles surrogate pairs correctly.
  
  size_t utf8_len = simdutf::utf16_length_in_utf8(reinterpret_cast<const char16_t*>(input.data()), input.size());
  std::string out;
  out.resize(utf8_len);

  simdutf::convert_utf16to8(
      reinterpret_cast<const char16_t*>(input.data()), 
      input.size(), 
      reinterpret_cast<char*>(out.data())
  );

  return checksum_bytes(out);
}
