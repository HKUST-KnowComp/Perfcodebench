#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline uint8_t blend_byte(uint32_t s, uint32_t d, uint32_t a) {
  uint32_t x = s * a + d * (255u - a) + 127u;
  return (uint8_t)((x + 128u + ((x + 128u) >> 8)) >> 8);
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* end = data + n;
  const uint8_t* end8 = data + (n & ~(size_t)7);
  
  while (data < end8) {
    hash ^= (uint64_t)data[0];
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)data[1];
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)data[2];
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)data[3];
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)data[4];
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)data[5];
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)data[6];
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)data[7];
    hash *= 1099511628211ULL;
    data += 8;
  }
  
  while (data < end) {
    hash ^= (uint64_t)*data++;
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  size_t bytes = pixels * 4u;
  uint64_t hash = 0;
  
  uint8_t* out = (uint8_t*)malloc(bytes);
  if (!out) return 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* restrict sp = src;
    const uint8_t* restrict dp = dst;
    uint8_t* restrict op = out;
    const uint8_t* sp_end = src + bytes;
    
    while (sp < sp_end) {
      uint32_t a = sp[3];
      uint32_t inv_a = 255u - a;
      
      uint32_t x0 = (uint32_t)sp[0] * a + (uint32_t)dp[0] * inv_a + 127u;
      uint32_t x1 = (uint32_t)sp[1] * a + (uint32_t)dp[1] * inv_a + 127u;
      uint32_t x2 = (uint32_t)sp[2] * a + (uint32_t)dp[2] * inv_a + 127u;
      
      op[0] = (uint8_t)((x0 + 128u + ((x0 + 128u) >> 8)) >> 8);
      op[1] = (uint8_t)((x1 + 128u + ((x1 + 128u) >> 8)) >> 8);
      op[2] = (uint8_t)((x2 + 128u + ((x2 + 128u) >> 8)) >> 8);
      op[3] = 255u;
      
      sp += 4;
      dp += 4;
      op += 4;
    }
    
    hash = checksum_bytes(out, bytes);
  }
  
  free(out);
  return hash;
}