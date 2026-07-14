#include "interface.h"

#include <simdutf.h>

#include <cstdint>
#include <string>

uint64_t transcode_checksum(const std::u16string& input) {
  const char16_t* data = input.data();
  const size_t len = input.size();

  const size_t utf8_len = simdutf::utf8_length_from_utf16le(data, len);
  std::string out;
  out.resize(utf8_len);

  simdutf::convert_valid_utf16le_to_utf8(data, len, out.data());

  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* bytes = reinterpret_cast<const unsigned char*>(out.data());
  for (size_t i = 0; i < utf8_len; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}
