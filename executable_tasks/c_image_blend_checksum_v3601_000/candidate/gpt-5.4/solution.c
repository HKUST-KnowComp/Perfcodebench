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

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* s = src;
    const uint8_t* d = dst;
    uint8_t* o = out;

    for (size_t i = 0; i < pixels; ++i) {
      uint32_t a = s[3];
      uint32_t ia = 255u - a;

      o[0] = (uint8_t)((s[0] * a + d[0] * ia + 127u) / 255u);
      o[1] = (uint8_t)((s[1] * a + d[1] * ia + 127u) / 255u);
      o[2] = (uint8_t)((s[2] * a + d[2] * ia + 127u) / 255u);
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
