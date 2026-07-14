#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>
#include <string>

namespace {
uint64_t checksum_utf32(const char32_t* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}
}

uint64_t transcode_checksum(const std::string& input) {
  std::vector<char32_t> buffer(input.size());
  size_t output_length = simdutf::transcode_utf8_to_utf32(input.data(), input.size(), buffer.data());
  buffer.resize(output_length);
  return checksum_utf32(buffer.data(), output_length);
}