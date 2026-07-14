#include "interface.h"

#include <string>
#include <cstdint>
#include <simdutf.h>

uint64_t transcode_checksum(const std::u16string& input) {
  size_t utf8_len = simdutf::utf8_length_from_utf16le(input.data(), input.size());
  std::string utf8(utf8_len, '\0');
  simdutf::convert_utf16le_to_utf8(input.data(), input.size(), utf8.data());
  
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char byte : utf8) {
    hash ^= static_cast<uint64_t>(byte);
    hash *= 1099511628211ULL;
  }
  return hash;
}
