#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return (uint8_t)(((uint32_t)s * (uint32_t)a + (uint32_t)d * (uint32_t)(255u - a) + 127u) / 255u);
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    const uint8_t* sp = src;
    const uint8_t* dp = dst;
    for (size_t i = 0; i < pixels; ++i) {
      uint8_t a = sp[3];
      uint8_t r = blend_byte(sp[0], dp[0], a);
      uint8_t g = blend_byte(sp[1], dp[1], a);
      uint8_t b = blend_byte(sp[2], dp[2], a);
      uint8_t oa = 255;
      h ^= r; h *= 1099511628211ULL;
      h ^= g; h *= 1099511628211ULL;
      h ^= b; h *= 1099511628211ULL;
      h ^= oa; h *= 1099511628211ULL;
      sp += 4;
      dp += 4;
    }
    hash = h;
  }
  return hash;
}