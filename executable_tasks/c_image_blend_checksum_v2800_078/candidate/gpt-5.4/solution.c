#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
  uint32_t sa = (uint32_t)s * (uint32_t)a;
  uint32_t dნა = (uint32_t)d * (uint32_t)(255u - a);
  return (uint8_t)((sa + dნა + 127u) / 255u);
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  size_t nbytes = pixels * 4u;
  uint64_t hash = 0;

  if (iters <= 0) return 0;

  uint8_t* out = (uint8_t*)malloc(nbytes);
  if (!out) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* s = src;
    const uint8_t* d = dst;
    uint8_t* o = out;

    uint64_t h = 1469598103934665603ULL;

    for (size_t i = 0; i < pixels; ++i) {
      uint8_t a = s[3];

      uint8_t r = blend_byte_fast(s[0], d[0], a);
      uint8_t g = blend_byte_fast(s[1], d[1], a);
      uint8_t b = blend_byte_fast(s[2], d[2], a);

      o[0] = r;
      o[1] = g;
      o[2] = b;
      o[3] = 255u;

      h ^= (uint64_t)r;
      h *= 1099511628211ULL;
      h ^= (uint64_t)g;
      h *= 1099511628211ULL;
      h ^= (uint64_t)b;
      h *= 1099511628211ULL;
      h ^= 255ULL;
      h *= 1099511628211ULL;

      s += 4;
      d += 4;
      o += 4;
    }

    hash = h;
  }

  free(out);
  return hash;
}
