#include "interface.h"

#include <vector>
#include <simdutf.h>

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
  // alloc worst-case number of UTF-16 code units (≤ input.size())
  std::vector<char16_t> out(input.size());
  // convert valid UTF-8 directly to UTF-16LE
  size_t out_len = simdutf::convert_utf8_to_utf16le(
      input.data(), input.size(), out.data());
  out.resize(out_len);
  return checksum_utf16(out.data(), out_len);
}