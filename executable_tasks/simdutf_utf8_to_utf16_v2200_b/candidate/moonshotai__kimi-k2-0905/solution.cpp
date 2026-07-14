#include "interface.h"

#include <simdutf.h>
#include <vector>

namespace {

uint64_t checksum_utf16(const char16_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const char16_t* end = data + len;
  while (data != end) {
    hash ^= static_cast<uint64_t>(*data++);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
  const size_t max_utf16 = simdutf::utf16_length_from_utf8(input.data(), input.size());
  std::vector<char16_t> out(max_utf16);

  size_t written = simdutf::convert_utf8_to_utf16le(
      input.data(), input.size(), reinterpret_cast<char16_t*>(out.data()));

  return checksum_utf16(out.data(), written);
}