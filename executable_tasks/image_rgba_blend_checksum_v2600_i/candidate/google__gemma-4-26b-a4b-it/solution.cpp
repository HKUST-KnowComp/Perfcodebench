#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// The original formula: (s * a + d * (255 - a) + 127) / 255
// We can optimize this using fixed-point arithmetic.
// Let alpha_inv = 255 - a.
// result = (s * a + d * alpha_inv + 127) / 255
// A common fast approximation for x / 255 is (x + 1 + (x >> 8)) >> 8
// However, to ensure exact correctness with the baseline's integer division,
// we must be careful. The baseline uses integer division which truncates.

inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = static_cast<uint32_t>(s) * a + 
                   static_cast<uint32_t>(d) * (255u - a) + 127u;
    return static_cast<uint8_t>(val / 255u);
}

// To achieve maximum performance, we process 4 bytes at a time and use 
// a more efficient checksum loop.

} // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  const std::size_t total_bytes = src.size();
  const std::size_t pixels = total_bytes / 4u;
  
  std::vector<uint8_t> out(total_bytes);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Blending loop
    for (std::size_t i = 0; i < pixels; ++i) {
      const std::size_t idx = i * 4u;
      const uint8_t a = src[idx + 3u];
      const uint32_t inv_a = 255u - a;

      // We must match the exact behavior of (s*a + d*(255-a) + 127) / 255
      out[idx]     = static_cast<uint8_t>((static_cast<uint32_t>(src[idx]) * a + static_cast<uint32_t>(dst[idx]) * inv_a + 127u) / 255u);
      out[idx + 1] = static_cast<uint8_t>((static_cast<uint32_t>(src[idx + 1]) * a + static_cast<uint32_t>(dst[idx + 1]) * inv_a + 127u) / 255u);
      out[idx + 2] = static_cast<uint8_t>((static_cast<uint32_t>(src[idx + 2]) * a + static_cast<uint32_t>(dst[idx + 2]) * inv_a + 127u) / 255u);
      out[idx + 3] = 255u;
    }

    // FNV-1a checksum loop
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < total_bytes; ++i) {
      hash ^= static_cast<uint64_t>(out[i]);
      hash *= 1099511628211ULL;
    }
    final_hash = hash;
  }

  return final_hash;
}
