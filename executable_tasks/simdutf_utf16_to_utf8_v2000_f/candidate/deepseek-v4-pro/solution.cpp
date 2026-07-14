#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <cstddef>
#include <vector>

namespace {

uint64_t checksum_bytes(const uint8_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  // handle empty input explicitly to avoid passing null to simdutf
  if (input.empty()) {
    return 1469598103934665603ULL;
  }
  // safe upper bound: each code unit can produce up to 4 bytes (surrogate pairs)
  const size_t max_out = input.size() * 4 + 4;
  std::vector<char> buffer(max_out);
  const size_t output_len = simdutf::convert_utf16le_to_utf8(
      reinterpret_cast<const char16_t*>(input.data()), input.size(), buffer.data());
  return checksum_bytes(reinterpret_cast<const uint8_t*>(buffer.data()), output_len);
}