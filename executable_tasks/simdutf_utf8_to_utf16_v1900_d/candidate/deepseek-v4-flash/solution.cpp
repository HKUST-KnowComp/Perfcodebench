#include "interface.h"
#include <simdutf.h>
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
  if (input.empty()) {
    return checksum_utf16(nullptr, 0);
  }
  size_t max_len = simdutf::maximal_length_of_utf16_from_utf8(input.size());
  std::vector<char16_t> out(max_len);
  size_t out_len = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
  out.resize(out_len);
  return checksum_utf16(out.data(), out.size());
}
