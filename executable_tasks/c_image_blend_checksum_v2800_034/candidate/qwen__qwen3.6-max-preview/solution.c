#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const uint8_t* s = src;
    const uint8_t* d = dst;

    for (size_t i = 0; i < pixels; ++i) {
      uint8_t a = s[3];
      uint8_t inv_a = 255u - a;

      uint32_t v0 = (uint32_t)s[0] * a + (uint32_t)d[0] * inv_a;
      uint32_t v1 = (uint32_t)s[1] * a + (uint32_t)d[1] * inv_a;
      uint32_t v2 = (uint32_t)s[2] * a + (uint32_t)d[2] * inv_a;

      uint8_t r = (uint8_t)((v0 * 257u + 32768u) >> 16);
      uint8_t g = (uint8_t)((v1 * 257u + 32768u) >> 16);
      uint8_t b = (uint8_t)((v2 * 257u + 32768u) >> 16);

      hash ^= r; hash *= 1099511628211ULL;
      hash ^= g; hash *= 1099511628211ULL;
      hash ^= b; hash *= 1099511628211ULL;
      hash ^= 255u; hash *= 1099511628211ULL;

      s += 4;
      d += 4;
    }
  }
  return hash;
}