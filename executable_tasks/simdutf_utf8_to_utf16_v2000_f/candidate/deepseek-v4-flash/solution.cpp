#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

namespace {
uint64_t checksum_utf16(const char16_t* data, size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}
}

uint64_t transcode_checksum(const std::string& input) {
  // Allocate output buffer: worst case is all ASCII -> 1:1 ratio, so input.size() is safe.
  std::vector<char16_t> output(input.size());
  size_t out_len = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), output.data());
  output.resize(out_len);
  return checksum_utf16(output.data(), out_len);
}
