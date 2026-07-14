#include "interface.h"
#include <simdutf.h>
#include <vector>

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
  const size_t utf16_len = simdutf::utf8_to_utf16_length(input.data(), input.size());
  std::vector<char16_t> out;
  out.reserve(utf16_len);
  simdutf::convert_utf8_to_utf16(input.data(), input.size(), out.data());
  out.resize(utf16_len);
  return checksum_utf16(out);
}