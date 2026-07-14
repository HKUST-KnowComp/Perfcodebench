#include "interface.h"
#include "simdutf.h"

uint64_t transcode_checksum(const std::u16string& input) {
  size_t utf8_size = simdutf::utf8_length_from_utf16le(input.data(), input.size());
  std::string out(utf8_size, '\0');
  simdutf::convert_utf16le_to_utf8(input.data(), input.size(), &out[0]);
  return checksum_bytes(out);
}

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}