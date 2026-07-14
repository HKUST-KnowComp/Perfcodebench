#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// Exact rounded division by 255 using multiplication and shift.
// For x in [0, 65535], ((x + 128) * 257) >> 16 == (x + 127) / 255.
inline uint8_t blend_channel(uint8_t s, uint8_t d, uint8_t a) {
    const uint32_t inv_a = 255u - static_cast<uint32_t>(a);
    const uint32_t sa = static_cast<uint32_t>(s) * static_cast<uint32_t>(a);
    const uint32_t da = static_cast<uint32_t>(d) * inv_a;
    const uint32_t t = sa + da; // 0..65025
    const uint32_t v = (t + 128u) * 257u;
    return static_cast<uint8_t>(v >> 16);
}

}  // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  (void)width;
  (void)height;

  const std::size_t pixels = src.size() / 4u;
  const uint8_t* sp = src.data();
  const uint8_t* dp = dst.data();

  // FNV-1a 64-bit parameters
  const uint64_t fnv_offset = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;

  uint64_t last_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Rewind pointers each iteration
    const uint8_t* s = sp;
    const uint8_t* d = dp;

    uint64_t hash = fnv_offset;

    for (std::size_t i = 0; i < pixels; ++i) {
      const uint8_t a = s[3];

      // Blend R, G, B
      uint8_t r = blend_channel(s[0], d[0], a);
      hash ^= static_cast<uint64_t>(r);
      hash *= fnv_prime;

      uint8_t g = blend_channel(s[1], d[1], a);
      hash ^= static_cast<uint64_t>(g);
      hash *= fnv_prime;

      uint8_t b = blend_channel(s[2], d[2], a);
      hash ^= static_cast<uint64_t>(b);
      hash *= fnv_prime;

      // Alpha is set to 255
      hash ^= 255ull;
      hash *= fnv_prime;

      s += 4;
      d += 4;
    }

    last_hash = hash;
  }

  return last_hash;
}
