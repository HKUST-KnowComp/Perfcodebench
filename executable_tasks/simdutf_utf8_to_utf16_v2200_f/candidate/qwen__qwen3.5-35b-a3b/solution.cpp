#include "interface.h"

#include <vector>
#include <simdutf.h>

namespace {

uint64_t checksum_utf16(const char16_t* data, size_t count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < count; ++i) {
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
  std::vector<char16_t> buffer(2 * input.size());
  auto result = simdutf::utf8_to_utf16(input.data(), input.size(), buffer.data());
  if (result.error()) {
    return 0;
  }
  size_t n = result.count();
  return checksum_utf16(buffer.data(), n);
}