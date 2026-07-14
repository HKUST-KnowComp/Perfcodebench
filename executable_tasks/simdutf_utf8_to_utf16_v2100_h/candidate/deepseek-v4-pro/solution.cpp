#include "interface.h"
#include <simdutf.h>
#include <cstddef>
#include <vector>

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
  size_t out_len = simdutf::utf16_length_from_utf8(input.data(), input.size());
  std::vector<char16_t> out(out_len);
  simdutf::result r = simdutf::convert_utf8_to_utf16(input.data(), input.size(), out.data());
  if (r.count < out_len) out.resize(r.count);
  return checksum_utf16(out.data(), out.size());
}