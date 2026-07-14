#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
  uint32_t x = (uint32_t)s * (uint32_t)a + (uint32_t)d * (uint32_t)(255u - a) + 127u;
  return (uint8_t)(x / 255u);
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  size_t nbytes = pixels * 4u;
  uint64_t hash = 0;

  if (iters <= 0) return 0;
  if (pixels == 0) {
    uint64_t h = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) hash = h;
    return hash;
  }

  uint8_t* out = (uint8_t*)malloc(nbytes);
  if (!out) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* s = src;
    const uint8_t* d = dst;
    uint8_t* o = out;

    for (size_t i = 0; i < pixels; ++i) {
      uint8_t a = s[3];
      o[0] = blend_byte_fast(s[0], d[0], a);
      o[1] = blend_byte_fast(s[1], d[1], a);
      o[2] = blend_byte_fast(s[2], d[2], a);
      o[3] = 255u;
      s += 4;
      d += 4;
      o += 4;
    }

    uint64_t h = 1469598103934665603ULL;
    const uint8_t* p = out;
    const uint8_t* end = out + nbytes;
    while (p != end) {
      h ^= (uint64_t)(*p++);
      h *= 1099511628211ULL;
    }
    hash = h;
  }

  free(out);
  return hash;
}
