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
  size_t output_units = simdutf::utf16le_length_from_utf8(input.data(), input.size());
  std::vector<char16_t> out(output_units);
  simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
  return checksum_utf16(out);
}