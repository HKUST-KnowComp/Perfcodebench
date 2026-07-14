#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>
#include <string>

namespace {

uint64_t checksum_utf16(const char16_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  // Maximum expansion: each input byte can become 2 UTF-16 code units
  size_t max_len = input.size() * 2 + 2;
  std::vector<char16_t> out(max_len);
  size_t out_len = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
  // Input is assumed valid, but check for safety
  if (out_len == 0) {
    return 0;  // Should not happen for valid UTF-8
  }
  return checksum_utf16(out.data(), out_len);
}
