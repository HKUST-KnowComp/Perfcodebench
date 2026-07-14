#include "interface.h"

#include <simdutf.h>

#include <cstdint>
#include <string>

uint64_t transcode_checksum(const std::u16string& input) {
  const char16_t* data = reinterpret_cast<const char16_t*>(input.data());
  const size_t len = input.size();

  // Exact UTF-8 size for valid UTF-16 input.
  const size_t utf8_len = simdutf::utf8_length_from_utf16le(data, len);

  std::string out;
  out.resize(utf8_len);
  if (utf8_len != 0) {
    simdutf::convert_valid_utf16le_to_utf8(data, len, out.data());
  }

  // FNV-1a over the exact UTF-8 byte sequence.
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(out.data());
  const unsigned char* end = p + utf8_len;
  while (p != end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL;
  }
  return hash;
}
