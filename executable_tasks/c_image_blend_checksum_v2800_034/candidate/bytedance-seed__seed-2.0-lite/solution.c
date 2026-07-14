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
  uint8_t* out = (uint8_t*)malloc(pixels * 4U);
  if (!out) return 0;
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < pixels; ++i) {
      size_t idx = i * 4U;
      uint8_t a = src[idx + 3U];
      uint16_t inv_a = 255U - a;
      // Blend R channel
      uint32_t r = (uint32_t)src[idx] * a + (uint32_t)dst[idx] * inv_a + 127U;
      out[idx] = (uint8_t)((r * 257U) >> 16);
      // Blend G channel
      uint32_t g = (uint32_t)src[idx + 1U] * a + (uint32_t)dst[idx + 1U] * inv_a + 127U;
      out[idx + 1U] = (uint8_t)((g * 257U) >> 16);
      // Blend B channel
      uint32_t b = (uint32_t)src[idx + 2U] * a + (uint32_t)dst[idx + 2U] * inv_a + 127U;
      out[idx + 2U] = (uint8_t)((b * 257U) >> 16);
      // Set output alpha to 255, matches original implementation
      out[idx + 3U] = 255U;
    }
    hash = checksum_bytes(out, pixels * 4U);
  }
  free(out);
  return hash;
}