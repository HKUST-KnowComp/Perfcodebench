#include "interface.h"

#include <simdutf.h>
#include <string>
#include <cstdint>

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
  
  const char16_t* input_ptr = input.data();
  size_t input_len = input.size();
  
  size_t utf8_len = simdutf::utf8_length_from_utf16le(input_ptr, input_len);
  
  std::string out(utf8_len, '\0');
  
  simdutf::convert_utf16le_to_utf8(input_ptr, input_len, out.data());
  
  return checksum_bytes(out.data(), utf8_len);
}