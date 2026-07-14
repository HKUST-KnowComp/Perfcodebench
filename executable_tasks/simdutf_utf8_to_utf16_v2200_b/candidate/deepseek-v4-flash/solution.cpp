#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <cstddef>
#include "simdutf.h"

namespace {

// FNV-1a hash over char16_t array
uint64_t fnv1a_utf16(const char16_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  size_t in_len = input.size();
  if (in_len == 0) {
    return 1469598103934665603ULL;  // FNV-1a base offset for empty input
  }
  // Allocate output buffer with max possible size (input length)
  std::vector<char16_t> out(in_len);
  // Perform SIMD-accelerated transcoding
  size_t out_len = simdutf::convert_utf8_to_utf16le(input.data(), in_len, out.data());
  // Compute and return checksum
  return fnv1a_utf16(out.data(), out_len);
}