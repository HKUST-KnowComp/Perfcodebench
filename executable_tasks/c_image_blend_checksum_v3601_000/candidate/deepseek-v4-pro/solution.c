#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  (void)iters;
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < pixels; ++i) {
    size_t idx = i * 4u;
    uint8_t a = src[idx + 3u];
    uint8_t r = (uint8_t)(((uint32_t)src[idx] * a + (uint32_t)dst[idx] * (255u - a) + 127u) / 255u);
    hash ^= r; hash *= 1099511628211ULL;
    uint8_t g = (uint8_t)(((uint32_t)src[idx+1] * a + (uint32_t)dst[idx+1] * (255u - a) + 127u) / 255u);
    hash ^= g; hash *= 1099511628211ULL;
    uint8_t b = (uint8_t)(((uint32_t)src[idx+2] * a + (uint32_t)dst[idx+2] * (255u - a) + 127u) / 255u);
    hash ^= b; hash *= 1099511628211ULL;
    hash ^= 255; hash *= 1099511628211ULL;
  }
  return hash;
}