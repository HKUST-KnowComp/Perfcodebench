#include "interface.h"
#include <vector>
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
  // Worst-case: every byte becomes one char16_t (ASCII).
  std::vector<char16_t> out(input.size());
  size_t written = simdutf::convert_utf8_to_utf16le(
      input.data(), input.size(), out.data());
  out.resize(written);
  return checksum_utf16(out);
}
