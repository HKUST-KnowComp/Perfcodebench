#include "interface.h"

#include <stdint.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 0;
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* sp = src;
    const uint8_t* dp = dst;
    uint64_t h = FNV_OFFSET;

    for (size_t i = 0; i < pixels; ++i) {
      uint32_t a = (uint32_t)sp[3];
      uint32_t inva = 255u - a;

      // R
      uint32_t t = (uint32_t)sp[0] * a + (uint32_t)dp[0] * inva + 127u;
      uint8_t out_r = (uint8_t)(t / 255u);
      h ^= (uint64_t)out_r; h *= FNV_PRIME;

      // G
      t = (uint32_t)sp[1] * a + (uint32_t)dp[1] * inva + 127u;
      uint8_t out_g = (uint8_t)(t / 255u);
      h ^= (uint64_t)out_g; h *= FNV_PRIME;

      // B
      t = (uint32_t)sp[2] * a + (uint32_t)dp[2] * inva + 127u;
      uint8_t out_b = (uint8_t)(t / 255u);
      h ^= (uint64_t)out_b; h *= FNV_PRIME;

      // A is always 255
      h ^= 255u; h *= FNV_PRIME;

      sp += 4;
      dp += 4;
    }

    hash = h;
  }

  return hash;
}
