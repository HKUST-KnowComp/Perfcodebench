#include "interface.h"
#include "simdutf.h"

uint64_t transcode_checksum(const std::u16string& input) {
  std::string out(simdutf::utf16_to_utf8_size(input.data(), input.size()));
  simdutf::utf16_to_utf8(input.data(), input.size(), out.data());
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