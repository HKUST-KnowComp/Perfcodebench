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
  if (input.empty()) {
    return checksum_utf16({});
  }
  const size_t required_utf16_units = simdutf::utf16_length_from_utf8(input.data(), input.size());
  std::vector<char16_t> out(required_utf16_units);
  simdutf::convert_utf8_to_utf16(input.data(), input.size(), out.data());
  return checksum_utf16(out);
}