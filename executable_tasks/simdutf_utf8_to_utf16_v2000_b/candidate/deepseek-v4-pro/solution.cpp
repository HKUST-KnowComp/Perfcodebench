#include "interface.h"

#include <vector>
#include <cstdint>
#include <simdutf.h>

namespace {

uint64_t checksum_utf16(const std::vector<char16_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (char16_t ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  size_t out_len = simdutf::utf16_length_from_utf8(input.data(), input.size());
  std::vector<char16_t> out(out_len);
  simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
  return checksum_utf16(out);
}