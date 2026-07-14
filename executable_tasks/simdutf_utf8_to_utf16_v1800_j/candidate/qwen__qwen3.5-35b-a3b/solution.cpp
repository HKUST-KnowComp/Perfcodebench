#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include "simdutf.h"

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
  std::vector<char16_t> out(2 * input.size());
  size_t len = simdutf::utf8_to_utf16_le(input.data(), input.size(), out.data());
  return checksum_utf16(out.data(), len);
}