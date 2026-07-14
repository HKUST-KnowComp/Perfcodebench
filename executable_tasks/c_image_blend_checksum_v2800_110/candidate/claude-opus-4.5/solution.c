#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* restrict src, const uint8_t* restrict dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  size_t total_bytes = pixels * 4u;
  uint64_t hash = 0;
  
  uint8_t* out = (uint8_t*)malloc(total_bytes);
  if (!out) return 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* sp = src;
    const uint8_t* dp = dst;
    uint8_t* op = out;
    
    for (size_t i = 0; i < pixels; ++i) {
      uint32_t a = sp[3];
      uint32_t inv_a = 255u - a;
      
      uint32_t r = sp[0] * a + dp[0] * inv_a + 127u;
      uint32_t g = sp[1] * a + dp[1] * inv_a + 127u;
      uint32_t b = sp[2] * a + dp[2] * inv_a + 127u;
      
      op[0] = (uint8_t)(((r + 128u) * 257u) >> 16);
      op[1] = (uint8_t)(((g + 128u) * 257u) >> 16);
      op[2] = (uint8_t)(((b + 128u) * 257u) >> 16);
      op[3] = 255u;
      
      sp += 4;
      dp += 4;
      op += 4;
    }
    
    uint64_t h = 1469598103934665603ULL;
    const uint8_t* data = out;
    size_t n = total_bytes;
    size_t unroll = n & ~7u;
    
    for (size_t i = 0; i < unroll; i += 8) {
      h ^= (uint64_t)data[i];
      h *= 1099511628211ULL;
      h ^= (uint64_t)data[i + 1];
      h *= 1099511628211ULL;
      h ^= (uint64_t)data[i + 2];
      h *= 1099511628211ULL;
      h ^= (uint64_t)data[i + 3];
      h *= 1099511628211ULL;
      h ^= (uint64_t)data[i + 4];
      h *= 1099511628211ULL;
      h ^= (uint64_t)data[i + 5];
      h *= 1099511628211ULL;
      h ^= (uint64_t)data[i + 6];
      h *= 1099511628211ULL;
      h ^= (uint64_t)data[i + 7];
      h *= 1099511628211ULL;
    }
    
    for (size_t i = unroll; i < n; ++i) {
      h ^= (uint64_t)data[i];
      h *= 1099511628211ULL;
    }
    
    hash = h;
  }
  
  free(out);
  return hash;
}