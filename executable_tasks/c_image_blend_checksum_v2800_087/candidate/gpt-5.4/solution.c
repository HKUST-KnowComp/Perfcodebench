#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_blended_rgba(const uint8_t* src, const uint8_t* dst, size_t pixels) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  const uint8_t* s = src;
  const uint8_t* d = dst;
  size_t i = 0;

  for (; i < pixels; ++i, s += 4, d += 4) {
    uint32_t a = s[3];
    uint32_t inva = 255u - a;

    uint8_t r = (uint8_t)((s[0] * a + d[0] * inva + 127u) / 255u);
    hash ^= (uint64_t)r;
    hash *= prime;

    uint8_t g = (uint8_t)((s[1] * a + d[1] * inva + 127u) / 255u);
    hash ^= (uint64_t)g;
    hash *= prime;

    uint8_t b = (uint8_t)((s[2] * a + d[2] * inva + 127u) / 255u);
    hash ^= (uint64_t)b;
    hash *= prime;

    hash ^= 255ULL;
    hash *= prime;
  }

  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_blended_rgba(src, dst, pixels);
  }

  return hash;
}
