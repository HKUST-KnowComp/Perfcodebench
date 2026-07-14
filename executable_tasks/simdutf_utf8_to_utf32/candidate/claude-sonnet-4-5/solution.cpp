#include "interface.h"

#include <simdutf.h>
#include <vector>

namespace {

uint64_t checksum_utf32(const char32_t* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  size_t expected_length = simdutf::utf32_length_from_utf8(input.data(), input.size());
  std::vector<char32_t> out(expected_length);
  
  size_t actual_length = simdutf::convert_utf8_to_utf32(input.data(), input.size(), out.data());
  
  return checksum_utf32(out.data(), actual_length);
}
