#include "interface.h"
#include <simdutf.h>
#include <string>

namespace {

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  const char16_t* u16_ptr = input.data();
  const size_t u16_len = input.size();
  const size_t u8_len = simdutf::utf8_length_from_utf16le(u16_ptr, u16_len);
  std::string out(u8_len, '\0');
  simdutf::convert_utf16le_to_utf8(u16_ptr, u16_len, out.data());
  return checksum_bytes(out);
}