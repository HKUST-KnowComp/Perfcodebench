#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  (void)width;
  (void)height;
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash_iter = 1469598103934665603ULL;
    for (size_t i = 0; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint8_t a = src[idx + 3u];
      uint32_t inv_a = 255u - a;
      uint8_t r = (uint8_t)(((uint32_t)src[idx] * a + (uint32_t)dst[idx] * inv_a + 127u) / 255u);
      uint8_t g = (uint8_t)(((uint32_t)src[idx+1] * a + (uint32_t)dst[idx+1] * inv_a + 127u) / 255u);
      uint8_t b = (uint8_t)(((uint32_t)src[idx+2] * a + (uint32_t)dst[idx+2] * inv_a + 127u) / 255u);
      hash_iter ^= (uint64_t)r;
      hash_iter *= 1099511628211ULL;
      hash_iter ^= (uint64_t)g;
      hash_iter *= 1099511628211ULL;
      hash_iter ^= (uint64_t)b;
      hash_iter *= 1099511628211ULL;
      hash_iter ^= (uint64_t)255u;
      hash_iter *= 1099511628211ULL;
    }
    hash = hash_iter;
  }
  return hash;
}
