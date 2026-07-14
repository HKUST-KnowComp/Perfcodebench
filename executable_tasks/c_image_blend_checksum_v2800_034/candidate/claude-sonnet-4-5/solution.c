#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return (uint8_t)(((uint32_t)s * (uint32_t)a + (uint32_t)d * (uint32_t)(255u - a) + 127u) / 255u);
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  size_t bytes = pixels * 4u;
  uint64_t hash = 0;
  
  uint8_t* out = (uint8_t*)malloc(bytes);
  if (!out) return 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    size_t i = 0;
    size_t pixels_unroll = pixels & ~3u;
    
    for (; i < pixels_unroll; i += 4u) {
      size_t idx0 = i * 4u;
      size_t idx1 = idx0 + 4u;
      size_t idx2 = idx1 + 4u;
      size_t idx3 = idx2 + 4u;
      
      uint8_t a0 = src[idx0 + 3u];
      uint8_t a1 = src[idx1 + 3u];
      uint8_t a2 = src[idx2 + 3u];
      uint8_t a3 = src[idx3 + 3u];
      
      uint8_t r0 = blend_byte(src[idx0], dst[idx0], a0);
      uint8_t g0 = blend_byte(src[idx0 + 1u], dst[idx0 + 1u], a0);
      uint8_t b0 = blend_byte(src[idx0 + 2u], dst[idx0 + 2u], a0);
      
      uint8_t r1 = blend_byte(src[idx1], dst[idx1], a1);
      uint8_t g1 = blend_byte(src[idx1 + 1u], dst[idx1 + 1u], a1);
      uint8_t b1 = blend_byte(src[idx1 + 2u], dst[idx1 + 2u], a1);
      
      uint8_t r2 = blend_byte(src[idx2], dst[idx2], a2);
      uint8_t g2 = blend_byte(src[idx2 + 1u], dst[idx2 + 1u], a2);
      uint8_t b2 = blend_byte(src[idx2 + 2u], dst[idx2 + 2u], a2);
      
      uint8_t r3 = blend_byte(src[idx3], dst[idx3], a3);
      uint8_t g3 = blend_byte(src[idx3 + 1u], dst[idx3 + 1u], a3);
      uint8_t b3 = blend_byte(src[idx3 + 2u], dst[idx3 + 2u], a3);
      
      out[idx0] = r0;
      out[idx0 + 1u] = g0;
      out[idx0 + 2u] = b0;
      out[idx0 + 3u] = 255u;
      
      out[idx1] = r1;
      out[idx1 + 1u] = g1;
      out[idx1 + 2u] = b1;
      out[idx1 + 3u] = 255u;
      
      out[idx2] = r2;
      out[idx2 + 1u] = g2;
      out[idx2 + 2u] = b2;
      out[idx2 + 3u] = 255u;
      
      out[idx3] = r3;
      out[idx3 + 1u] = g3;
      out[idx3 + 2u] = b3;
      out[idx3 + 3u] = 255u;
      
      hash ^= (uint64_t)r0;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)g0;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)b0;
      hash *= 1099511628211ULL;
      hash ^= 255ULL;
      hash *= 1099511628211ULL;
      
      hash ^= (uint64_t)r1;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)g1;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)b1;
      hash *= 1099511628211ULL;
      hash ^= 255ULL;
      hash *= 1099511628211ULL;
      
      hash ^= (uint64_t)r2;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)g2;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)b2;
      hash *= 1099511628211ULL;
      hash ^= 255ULL;
      hash *= 1099511628211ULL;
      
      hash ^= (uint64_t)r3;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)g3;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)b3;
      hash *= 1099511628211ULL;
      hash ^= 255ULL;
      hash *= 1099511628211ULL;
    }
    
    for (; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint8_t a = src[idx + 3u];
      uint8_t r = blend_byte(src[idx], dst[idx], a);
      uint8_t g = blend_byte(src[idx + 1u], dst[idx + 1u], a);
      uint8_t b = blend_byte(src[idx + 2u], dst[idx + 2u], a);
      
      out[idx] = r;
      out[idx + 1u] = g;
      out[idx + 2u] = b;
      out[idx + 3u] = 255u;
      
      hash ^= (uint64_t)r;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)g;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)b;
      hash *= 1099511628211ULL;
      hash ^= 255ULL;
      hash *= 1099511628211ULL;
    }
  }
  
  free(out);
  return hash;
}