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
  uint64_t hash = 0;
  
  uint8_t* out = (uint8_t*)malloc(pixels * 4u);
  if (!out) return 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint8_t a = src[idx + 3u];
      
      // Inline blend_byte logic for R
      uint32_t s_r = src[idx];
      uint32_t d_r = dst[idx];
      uint32_t val_r = s_r * a + d_r * (255u - a) + 127u;
      out[idx] = (uint8_t)(val_r / 255u);
      
      // Inline blend_byte logic for G
      uint32_t s_g = src[idx + 1u];
      uint32_t d_g = dst[idx + 1u];
      uint32_t val_g = s_g * a + d_g * (255u - a) + 127u;
      out[idx + 1u] = (uint8_t)(val_g / 255u);
      
      // Inline blend_byte logic for B
      uint32_t s_b = src[idx + 2u];
      uint32_t d_b = dst[idx + 2u];
      uint32_t val_b = s_b * a + d_b * (255u - a) + 127u;
      out[idx + 2u] = (uint8_t)(val_b / 255u);
      
      out[idx + 3u] = 255u;
    }
    hash = checksum_bytes(out, pixels * 4u);
  }
  
  free(out);
  return hash;
}