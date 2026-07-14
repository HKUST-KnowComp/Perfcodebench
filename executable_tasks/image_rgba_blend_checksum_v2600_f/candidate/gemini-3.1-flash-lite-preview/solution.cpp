#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  const std::size_t num_pixels = static_cast<std::size_t>(width) * height;
  const std::size_t num_bytes = num_pixels * 4;
  std::vector<uint8_t> out(num_bytes);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < num_pixels; ++i) {
      const std::size_t idx = i * 4;
      const uint32_t a = src[idx + 3];
      const uint32_t inv_a = 255 - a;

      // Blend R, G, B using fixed-point arithmetic
      // (s * a + d * (255 - a) + 127) / 255 is equivalent to 
      // (s * a + d * (255 - a) + 128) >> 8 for standard alpha blending approximations
      // However, to match the exact requirement of the original formula:
      out[idx]     = static_cast<uint8_t>((static_cast<uint32_t>(src[idx]) * a + static_cast<uint32_t>(dst[idx]) * inv_a + 127) / 255);
      out[idx + 1] = static_cast<uint8_t>((static_cast<uint32_t>(src[idx + 1]) * a + static_cast<uint32_t>(dst[idx + 1]) * inv_a + 127) / 255);
      out[idx + 2] = static_cast<uint8_t>((static_cast<uint32_t>(src[idx + 2]) * a + static_cast<uint32_t>(dst[idx + 2]) * inv_a + 127) / 255);
      out[idx + 3] = 255;
    }

    // FNV-1a checksum
    uint64_t h = 1469598103934665603ULL;
    for (std::size_t i = 0; i < num_bytes; ++i) {
      h ^= static_cast<uint64_t>(out[i]);
      h *= 1099511628211ULL;
    }
    hash = h;
  }
  return hash;
}