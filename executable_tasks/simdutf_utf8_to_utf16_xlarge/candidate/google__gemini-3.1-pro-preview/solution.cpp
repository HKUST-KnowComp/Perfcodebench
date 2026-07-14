#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

/**
 * Computes the FNV-1a 64-bit hash over the UTF-16 code units.
 * This matches the logic in the baseline but is optimized for sequential access.
 */
inline uint64_t checksum_utf16(const char16_t* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }

  // 1. Determine the required size for the UTF-16 output.
  // simdutf provides an efficient way to calculate the number of 16-bit code units.
  size_t utf16_length = simdutf::utf16_length_from_utf8(input.data(), input.size());

  // 2. Allocate a buffer for the UTF-16 output.
  // Using a vector for automatic memory management.
  std::vector<char16_t> output_buffer(utf16_length);

  // 3. Perform the SIMD-accelerated transcoding.
  // convert_utf8_to_utf16 returns the number of char16_t written.
  size_t written = simdutf::convert_utf8_to_utf16(input.data(), input.size(), output_buffer.data());

  // 4. Compute the checksum over the exact UTF-16 sequence.
  return checksum_utf16(output_buffer.data(), written);
}