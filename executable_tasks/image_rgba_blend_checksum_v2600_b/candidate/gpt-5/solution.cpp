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

  // Preserve baseline behavior for iters <= 0
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t pixels = src.size() / 4u;
  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();

  // FNV-1a 64-bit
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;

  for (std::size_t i = 0; i < pixels; ++i) {
    const uint32_t a = s[3];
    const uint32_t inva = 255u - a;

    // R
    uint32_t t = static_cast<uint32_t>(s[0]) * a + static_cast<uint32_t>(d[0]) * inva + 127u;
    uint8_t out_r = static_cast<uint8_t>(t / 255u);
    hash ^= static_cast<uint64_t>(out_r);
    hash *= fnv_prime;

    // G
    t = static_cast<uint32_t>(s[1]) * a + static_cast<uint32_t>(d[1]) * inva + 127u;
    uint8_t out_g = static_cast<uint8_t>(t / 255u);
    hash ^= static_cast<uint64_t>(out_g);
    hash *= fnv_prime;

    // B
    t = static_cast<uint32_t>(s[2]) * a + static_cast<uint32_t>(d[2]) * inva + 127u;
    uint8_t out_b = static_cast<uint8_t>(t / 255u);
    hash ^= static_cast<uint64_t>(out_b);
    hash *= fnv_prime;

    // A is fully opaque (255)
    hash ^= 255u;
    hash *= fnv_prime;

    s += 4;
    d += 4;
  }

  return hash;
}
