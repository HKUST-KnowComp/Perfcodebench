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
  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();
  uint64_t hash = 0;
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    const uint8_t* sp = s;
    const uint8_t* dp = d;
    for (std::size_t i = 0; i < pixels; ++i) {
      uint32_t a = sp[3];
      uint32_t da = 255u - a;

      uint32_t r = static_cast<uint32_t>(sp[0]) * a + static_cast<uint32_t>(dp[0]) * da;
      uint32_t g = static_cast<uint32_t>(sp[1]) * a + static_cast<uint32_t>(dp[1]) * da;
      uint32_t b = static_cast<uint32_t>(sp[2]) * a + static_cast<uint32_t>(dp[2]) * da;

      // Exact replacement for (val + 127) / 255 in range [0, 65535]
      uint8_t br = static_cast<uint8_t>((r * 257u + 32768u) >> 16);
      uint8_t bg = static_cast<uint8_t>((g * 257u + 32768u) >> 16);
      uint8_t bb = static_cast<uint8_t>((b * 257u + 32768u) >> 16);

      hash = (hash ^ br) * FNV_PRIME;
      hash = (hash ^ bg) * FNV_PRIME;
      hash = (hash ^ bb) * FNV_PRIME;
      hash = (hash ^ 255u) * FNV_PRIME;

      sp += 4;
      dp += 4;
    }
  }
  return hash;
}
