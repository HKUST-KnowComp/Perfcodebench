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

  const std::size_t pixels = src.size() / 4u;
  const uint8_t* sp_base = src.data();
  const uint8_t* dp_base = dst.data();

  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* sp = sp_base;
    const uint8_t* dp = dp_base;

    uint64_t h = FNV_OFFSET_BASIS;

    std::size_t n = pixels;
    while (n--) {
      const uint32_t a = sp[3];
      const uint32_t inv = 255u - a;

      uint32_t t0 = static_cast<uint32_t>(sp[0]) * a + static_cast<uint32_t>(dp[0]) * inv;
      uint8_t o0 = static_cast<uint8_t>((t0 + 127u) / 255u);
      h ^= static_cast<uint64_t>(o0);
      h *= FNV_PRIME;

      uint32_t t1 = static_cast<uint32_t>(sp[1]) * a + static_cast<uint32_t>(dp[1]) * inv;
      uint8_t o1 = static_cast<uint8_t>((t1 + 127u) / 255u);
      h ^= static_cast<uint64_t>(o1);
      h *= FNV_PRIME;

      uint32_t t2 = static_cast<uint32_t>(sp[2]) * a + static_cast<uint32_t>(dp[2]) * inv;
      uint8_t o2 = static_cast<uint8_t>((t2 + 127u) / 255u);
      h ^= static_cast<uint64_t>(o2);
      h *= FNV_PRIME;

      h ^= 255u;
      h *= FNV_PRIME;

      sp += 4;
      dp += 4;
    }

    hash = h;
  }

  return hash;
}
