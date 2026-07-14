#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  if (iters <= 0) return 0;
  
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 1469598103934665603ULL;
  
  for (size_t i = 0; i < pixels; ++i) {
    size_t idx = i * 4u;
    uint32_t a = src[idx + 3u];
    uint32_t inv_a = 255u - a;
    
    uint32_t r_t = src[idx] * a + dst[idx] * inv_a + 127u;
    uint8_t r = (uint8_t)(r_t / 255u);
    hash ^= r;
    hash *= 1099511628211ULL;
    
    uint32_t g_t = src[idx + 1u] * a + dst[idx + 1u] * inv_a + 127u;
    uint8_t g = (uint8_t)(g_t / 255u);
    hash ^= g;
    hash *= 1099511628211ULL;
    
    uint32_t b_t = src[idx + 2u] * a + dst[idx + 2u] * inv_a + 127u;
    uint8_t b = (uint8_t)(b_t / 255u);
    hash ^= b;
    hash *= 1099511628211ULL;
    
    hash ^= 255u;
    hash *= 1099511628211ULL;
  }
  
  return hash;
}
