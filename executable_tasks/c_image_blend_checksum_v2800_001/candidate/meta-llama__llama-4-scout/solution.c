#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint32_t blend_pixel(uint32_t s, uint32_t d, uint8_t a) {
  uint32_t sa = (s & 0xFF00FF00) * a;
  uint32_t da = (d & 0xFF00FF00) * (255u - a);
  uint32_t sb = (s & 0x00FF00FF) * a;
  uint32_t db = (d & 0x00FF00FF) * (255u - a);
  return ((sa + da + 0x80000000) >> 8 & 0xFF00FF00) | ((sb + db + 0x8000) >> 8 & 0x00FF00FF);
}

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
  uint64_t hash = 0;
  uint32_t* out = (uint32_t*)malloc(pixels * sizeof(uint32_t));
  if (!out) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < pixels; ++i) {
      uint32_t s = *((uint32_t*)(src + i * 4u));
      uint32_t d = *((uint32_t*)(dst + i * 4u));
      uint8_t a = src[i * 4u + 3u];
      out[i] = blend_pixel(s, d, a);
    }
    hash = checksum_bytes((uint8_t*)out, pixels * sizeof(uint32_t));
  }
  free(out);
  return hash;
}