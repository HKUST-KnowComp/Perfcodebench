#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_blended(const uint8_t* src, const uint8_t* dst, size_t pixels) {
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET;
  size_t n = pixels * 4u;

  const uint8_t* s = src;
  const uint8_t* d = dst;
  size_t i = 0;

  for (; i < pixels; ++i, s += 4, d += 4) {
    uint32_t a = s[3];
    uint32_t ia = 255u - a;

    uint8_t r = (uint8_t)((s[0] * a + d[0] * ia + 127u) / 255u);
    hash ^= (uint64_t)r;
    hash *= FNV_PRIME;

    uint8_t g = (uint8_t)((s[1] * a + d[1] * ia + 127u) / 255u);
    hash ^= (uint64_t)g;
    hash *= FNV_PRIME;

    uint8_t b = (uint8_t)((s[2] * a + d[2] * ia + 127u) / 255u);
    hash ^= (uint64_t)b;
    hash *= FNV_PRIME;

    hash ^= 255u;
    hash *= FNV_PRIME;
  }

  (void)n;
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_blended(src, dst, pixels);
  }

  return hash;
}
