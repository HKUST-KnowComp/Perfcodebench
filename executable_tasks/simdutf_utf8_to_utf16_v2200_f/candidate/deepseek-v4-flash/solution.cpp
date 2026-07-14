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
  size_t utf16_length = simdutf::utf16_length_from_utf8(input.data(), input.size());
  std::vector<char16_t> output(utf16_length);
  size_t written = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), output.data());
  return checksum_utf16(output.data(), written);
}