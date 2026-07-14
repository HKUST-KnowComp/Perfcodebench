#include "interface.h"

#include <simdutf.h>
#include <string>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return checksum_bytes("", 0);
  }

  size_t expected_len = simdutf::utf8_length_from_utf16le(
      reinterpret_cast<const char16_t*>(input.data()), input.size());
  
  std::string out(expected_len, '\0');
  
  size_t actual_len = simdutf::convert_utf16le_to_utf8(
      reinterpret_cast<const char16_t*>(input.data()), input.size(),
      out.data());
  
  out.resize(actual_len);
  
  return checksum_bytes(out.data(), out.size());
}
