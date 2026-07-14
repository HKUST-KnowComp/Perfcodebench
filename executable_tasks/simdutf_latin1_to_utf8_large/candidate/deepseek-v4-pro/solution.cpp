#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <string>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    unsigned char ch = static_cast<unsigned char>(data[i]);
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }
  size_t utf8_len = simdutf::utf8_length_from_latin1(input.data(), input.size());
  std::string utf8;
  utf8.resize(utf8_len);
  size_t written = simdutf::convert_latin1_to_utf8(input.data(), input.size(), utf8.data());
  // written should equal utf8_len for valid Latin-1
  return checksum_bytes(utf8.data(), written);
}