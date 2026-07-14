#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  size_t nbytes = pixels * 4u;
  uint64_t hash = 0;

  if (iters <= 0) return 0;

  uint8_t* out = (uint8_t*)malloc(nbytes);
  if (!out) return 0;

  static uint16_t blend_lut[256][256];
  static int lut_ready = 0;
  if (!lut_ready) {
    for (unsigned a = 0; a < 256; ++a) {
      unsigned inva = 255u - a;
      for (unsigned x = 0; x < 256; ++x) {
        blend_lut[a][x] = (uint16_t)(x * inva);
      }
    }
    lut_ready = 1;
  }

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* s = src;
    const uint8_t* d = dst;
    uint8_t* o = out;

    for (size_t i = 0; i < pixels; ++i) {
      uint32_t a = s[3];
      const uint16_t* lut = blend_lut[a];

      o[0] = (uint8_t)(((uint32_t)s[0] * a + (uint32_t)lut[d[0]] + 127u) / 255u);
      o[1] = (uint8_t)(((uint32_t)s[1] * a + (uint32_t)lut[d[1]] + 127u) / 255u);
      o[2] = (uint8_t)(((uint32_t)s[2] * a + (uint32_t)lut[d[2]] + 127u) / 255u);
      o[3] = 255u;

      s += 4;
      d += 4;
      o += 4;
    }

    hash = checksum_bytes(out, nbytes);
  }

  free(out);
  return hash;
}
