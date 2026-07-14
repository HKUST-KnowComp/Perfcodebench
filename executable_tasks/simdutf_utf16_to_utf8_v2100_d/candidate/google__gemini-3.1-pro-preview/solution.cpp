#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

/**
 * Computes the 64-bit FNV-1a hash of the given byte sequence.
 * This matches the logic used in the baseline implementation.
 */
inline uint64_t calculate_fnv1a(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return calculate_fnv1a(nullptr, 0);
  }

  // 1. Determine the required size for the UTF-8 output buffer.
  // simdutf provides an optimized way to calculate the exact length.
  size_t utf8_len = simdutf::utf8_length_from_utf16le(input.data(), input.size());

  // 2. Allocate a buffer for the UTF-8 output.
  // Using a vector for automatic memory management and alignment.
  std::vector<char> buffer(utf8_len);

  // 3. Perform the SIMD-accelerated transcoding.
  // convert_utf16le_to_utf8 returns the number of bytes written.
  size_t written = simdutf::convert_utf16le_to_utf8(input.data(), input.size(), buffer.data());

  // 4. Calculate the FNV-1a checksum on the resulting UTF-8 bytes.
  return calculate_fnv1a(buffer.data(), written);
}
