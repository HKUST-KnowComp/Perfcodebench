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
  if (iters <= 0) return 0;

  const std::size_t pixels = src.size() / 4u;
  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();

  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  for (std::size_t i = 0; i < pixels; ++i) {
    const std::size_t idx = i * 4u;
    const uint32_t a = s[idx + 3u];
    const uint32_t inv_a = 255u - a;

    uint32_t r = s[idx] * a + d[idx] * inv_a + 127u;
    hash ^= static_cast<uint8_t>((r + 1u + (r >> 8u)) >> 8u);
    hash *= prime;

    uint32_t g = s[idx + 1u] * a + d[idx + 1u] * inv_a + 127u;
    hash ^= static_cast<uint8_t>((g + 1u + (g >> 8u)) >> 8u);
    hash *= prime;

    uint32_t b = s[idx + 2u] * a + d[idx + 2u] * inv_a + 127u;
    hash ^= static_cast<uint8_t>((b + 1u + (b >> 8u)) >> 8u);
    hash *= prime;

    hash ^= 255u;
    hash *= prime;
  }
  return hash;
}
