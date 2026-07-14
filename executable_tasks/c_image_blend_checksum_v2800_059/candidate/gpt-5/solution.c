#include "interface.h"
#include <stdint.h>

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  unsigned inva = 255u - (unsigned)a;
  unsigned t = (unsigned)s * (unsigned)a + (unsigned)d * inva;
  t += 127u;
  return (uint8_t)(t / 255u);
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  if (iters <= 0) return 0;

  size_t pixels = (size_t)width * (size_t)height;

  const uint8_t* s = src;
  const uint8_t* d = dst;

  uint64_t hash = 1469598103934665603ULL; /* FNV-1 64-bit offset */
  const uint64_t prime = 1099511628211ULL; /* FNV-1 64-bit prime */

  for (size_t i = 0; i < pixels; ++i) {
    uint8_t a = s[3];
    uint8_t r = blend_byte(s[0], d[0], a);
    uint8_t g = blend_byte(s[1], d[1], a);
    uint8_t b = blend_byte(s[2], d[2], a);

    /* Stream FNV-1 over the output RGBA bytes (alpha = 255) */
    hash ^= (uint64_t)r; hash *= prime;
    hash ^= (uint64_t)g; hash *= prime;
    hash ^= (uint64_t)b; hash *= prime;
    hash ^= 255u;        hash *= prime;

    s += 4;
    d += 4;
  }

  return hash;
}
