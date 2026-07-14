#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

namespace {

/**
 * Computes the FNV-1a 64-bit hash over a byte buffer.
 * FNV-1a is sequential, but compilers can often unroll this loop.
 */
inline uint64_t compute_fnv1a(const uint8_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }

  const char16_t* src = reinterpret_cast<const char16_t*>(input.data());
  size_t src_len = input.length();

  // 1. Determine the required buffer size for UTF-8 output.
  // simdutf provides an efficient way to calculate this.
  size_t utf8_len = simdutf::utf8_length_from_utf16le(src, src_len);

  // 2. Allocate a buffer for the UTF-8 output.
  // Using a vector for automatic memory management; for very small strings, 
  // a stack buffer could be faster, but input size is unknown.
  std::vector<uint8_t> buffer(utf8_len);

  // 3. Perform the SIMD-accelerated transcoding.
  size_t written = simdutf::convert_utf16le_to_utf8(src, src_len, reinterpret_cast<char*>(buffer.data()));

  // 4. Compute the FNV-1a checksum over the resulting UTF-8 bytes.
  return compute_fnv1a(buffer.data(), written);
}
