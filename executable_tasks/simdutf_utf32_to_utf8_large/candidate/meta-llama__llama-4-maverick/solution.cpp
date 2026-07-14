#include "interface.h"
#include "simdutf.h"

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
  size_t expected_size = simdutf::utf8_length_from_utf32(input.data(), input.size());
  std::string out(expected_size, '\0');
  simdutf::convert_utf32_to_utf8(input.data(), input.size(), &out[0]);
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : out) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}