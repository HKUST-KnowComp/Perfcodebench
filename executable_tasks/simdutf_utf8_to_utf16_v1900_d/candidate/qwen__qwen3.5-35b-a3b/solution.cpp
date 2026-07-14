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
  std::vector<char16_t> out(input.size() * 2);
  size_t len = simdutf::utf8_to_utf16_le(input.data(), input.size(), out.data());
  out.resize(len);
  return checksum_utf16(out);
}