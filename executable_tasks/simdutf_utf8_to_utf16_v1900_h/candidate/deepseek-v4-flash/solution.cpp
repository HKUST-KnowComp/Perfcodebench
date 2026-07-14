#include "interface.h"
#include "simdutf.h"

#include <cstdint>
#include <string>
#include <vector>

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
  // Allocate enough space for the UTF-16 output (max 1 char16_t per input byte)
  std::vector<char16_t> out(input.size());
  // Convert using the SIMD-optimized simdutf library
  size_t written = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
  // Compute and return checksum over the exact number of produced code units
  return checksum_utf16(out.data(), written);
}
