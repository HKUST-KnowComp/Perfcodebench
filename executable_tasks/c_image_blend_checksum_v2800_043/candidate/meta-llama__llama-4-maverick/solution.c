#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return (uint8_t)(((s * a + d * (255 - a)) + 127) / 255);
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  uint8_t* out = (uint8_t*)malloc(pixels * 4u);
  if (!out) return 0;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint8_t a = src[idx + 3];
      out[idx] = blend_byte(src[idx], dst[idx], a);
      out[idx + 1] = blend_byte(src[idx + 1], dst[idx + 1], a);
      out[idx + 2] = blend_byte(src[idx + 2], dst[idx + 2], a);
      out[idx + 3] = 255;
    }
    hash = checksum_bytes(out, pixels * 4u);
  }
  free(out);
  return hash;
}
