#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  uint16_t tmp = (uint16_t)s * (uint16_t)a + (uint16_t)d * (uint16_t)(255u - a) + 128u;
  return (uint8_t)(tmp / 255u);
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; i += 8) {
    uint64_t w = *(const uint64_t*)(data + i);
    hash ^= w;
    hash *= 1099511628211ULL;
  }
  // Handle the remaining bytes
  for (size_t i = n & ~7; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 0;
  uint8_t* out = (uint8_t*)malloc(pixels * 4u);
  if (!out) return 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint8_t a = src[idx + 3u];
      out[idx] = blend_byte(src[idx], dst[idx], a);
      out[idx + 1u] = blend_byte(src[idx + 1u], dst[idx + 1u], a);
      out[idx + 2u] = blend_byte(src[idx + 2u], dst[idx + 2u], a);
      out[idx + 3u] = 255u;
    }
    hash = checksum_bytes(out, pixels * 4u);
  }
  free(out);
  return hash;
}