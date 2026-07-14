#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
  uint32_t t = (uint32_t)s * (uint32_t)a + (uint32_t)d * (uint32_t)(255u - a) + 127u;
  return (uint8_t)(t / 255u);
}

static inline uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  size_t bytes = pixels * 4u;
  uint64_t hash = 0;

  if (iters <= 0) return 0;

  uint8_t* out = (uint8_t*)malloc(bytes);
  if (!out) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* s = src;
    const uint8_t* d = dst;
    uint8_t* o = out;

    size_t i = 0;
    for (; i + 4 <= pixels; i += 4) {
      uint8_t a0 = s[3];
      o[0] = blend_byte_fast(s[0], d[0], a0);
      o[1] = blend_byte_fast(s[1], d[1], a0);
      o[2] = blend_byte_fast(s[2], d[2], a0);
      o[3] = 255u;

      uint8_t a1 = s[7];
      o[4] = blend_byte_fast(s[4], d[4], a1);
      o[5] = blend_byte_fast(s[5], d[5], a1);
      o[6] = blend_byte_fast(s[6], d[6], a1);
      o[7] = 255u;

      uint8_t a2 = s[11];
      o[8] = blend_byte_fast(s[8], d[8], a2);
      o[9] = blend_byte_fast(s[9], d[9], a2);
      o[10] = blend_byte_fast(s[10], d[10], a2);
      o[11] = 255u;

      uint8_t a3 = s[15];
      o[12] = blend_byte_fast(s[12], d[12], a3);
      o[13] = blend_byte_fast(s[13], d[13], a3);
      o[14] = blend_byte_fast(s[14], d[14], a3);
      o[15] = 255u;

      s += 16;
      d += 16;
      o += 16;
    }

    for (; i < pixels; ++i) {
      uint8_t a = s[3];
      o[0] = blend_byte_fast(s[0], d[0], a);
      o[1] = blend_byte_fast(s[1], d[1], a);
      o[2] = blend_byte_fast(s[2], d[2], a);
      o[3] = 255u;
      s += 4;
      d += 4;
      o += 4;
    }

    hash = checksum_bytes(out, bytes);
  }

  free(out);
  return hash;
}
