#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  if (iters <= 0) return 0;
  
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* s = src;
  const uint8_t* d = dst;
  
  for (size_t i = 0; i < pixels; ++i) {
    uint8_t a = s[3];
    uint8_t inv_a = 255u - a;
    
    uint32_t r = (uint32_t)s[0] * a + (uint32_t)d[0] * inv_a + 127u;
    uint32_t g = (uint32_t)s[1] * a + (uint32_t)d[1] * inv_a + 127u;
    uint32_t b = (uint32_t)s[2] * a + (uint32_t)d[2] * inv_a + 127u;
    
    hash ^= (uint8_t)(r / 255u); hash *= 1099511628211ULL;
    hash ^= (uint8_t)(g / 255u); hash *= 1099511628211ULL;
    hash ^= (uint8_t)(b / 255u); hash *= 1099511628211ULL;
    hash ^= (uint8_t)255u;       hash *= 1099511628211ULL;
    
    s += 4;
    d += 4;
  }
  return hash;
}
