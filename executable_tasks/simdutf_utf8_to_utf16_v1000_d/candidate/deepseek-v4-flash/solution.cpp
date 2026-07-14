#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

namespace {

uint64_t checksum_utf16(const char16_t* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  std::vector<char16_t> buffer(input.size() * 2);  // safe upper bound
  size_t len = simdutf::convert_utf8_to_utf16(input.data(), input.size(), buffer.data());
  return checksum_utf16(buffer.data(), len);
}
