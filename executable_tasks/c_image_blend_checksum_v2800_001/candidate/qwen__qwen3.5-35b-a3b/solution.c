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
  (void)width;
  (void)height;
  size_t pixels = (size_t)width * (size_t)height;
  uint8_t* out = (uint8_t*)malloc(pixels * 4u);
  if (!out) return 0;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* s = src;
    const uint8_t* d = dst;
    uint8_t* o = out;
    for (size_t i = 0; i < pixels; ++i) {
      uint8_t a = s[3];
      uint8_t inv_a = 255u - a;
      o[0] = (uint8_t)(((uint32_t)s[0] * a + (uint32_t)d[0] * inv_a + 127u) / 255u);
      o[1] = (uint8_t)(((uint32_t)s[1] * a + (uint32_t)d[1] * inv_a + 127u) / 255u);
      o[2] = (uint8_t)(((uint32_t)s[2] * a + (uint32_t)d[2] * inv_a + 127u) / 255u);
      o[3] = 255u;
      s += 4;
      d += 4;
      o += 4;
    }
    hash = checksum_bytes(out, pixels * 4u);
  }
  free(out);
  return hash;
}