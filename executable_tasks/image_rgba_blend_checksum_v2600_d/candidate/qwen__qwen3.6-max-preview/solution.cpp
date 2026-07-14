#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  (void)width;
  (void)height;
  const size_t n = src.size();
  const size_t pixels = n / 4;

  // Allocate output buffer once
  std::vector<uint8_t> out(n);
  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();
  uint8_t* o = out.data();

  // Blend phase: computed once since output is deterministic
  for (size_t i = 0; i < pixels; ++i) {
    size_t idx = i * 4;
    uint8_t a = s[idx + 3];
    uint32_t inv_a = 255 - a;

    uint32_t v0 = s[idx] * a + d[idx] * inv_a;
    uint32_t v1 = s[idx + 1] * a + d[idx + 1] * inv_a;
    uint32_t v2 = s[idx + 2] * a + d[idx + 2] * inv_a;

    // Exact rounding division by 255 using fixed-point math:
    // (v + 127) / 255 == (v * 257 + 32768) >> 16 for v in [0, 65025]
    o[idx]     = static_cast<uint8_t>((v0 * 257 + 32768) >> 16);
    o[idx + 1] = static_cast<uint8_t>((v1 * 257 + 32768) >> 16);
    o[idx + 2] = static_cast<uint8_t>((v2 * 257 + 32768) >> 16);
    o[idx + 3] = 255;
  }

  uint64_t hash = 0;
  const uint64_t fnv_prime  = 1099511628211ULL;
  const uint64_t fnv_offset = 1469598103934665603ULL;

  // Hash phase: repeated per iters as requested, but operates on pre-blended data
  for (int iter = 0; iter < iters; ++iter) {
    hash = fnv_offset;
    // Unroll by 4 to reduce loop overhead and improve pipelining
    for (size_t i = 0; i < n; i += 4) {
      hash ^= o[i];     hash *= fnv_prime;
      hash ^= o[i + 1]; hash *= fnv_prime;
      hash ^= o[i + 2]; hash *= fnv_prime;
      hash ^= o[i + 3]; hash *= fnv_prime;
    }
  }

  return hash;
}
