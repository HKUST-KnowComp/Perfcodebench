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
    uint64_t h = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
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
      
      uint8_t b0, b1, b2;
      
      b0 = blend_byte(src[idx0], dst[idx0], a0);
      b1 = blend_byte(src[idx0 + 1u], dst[idx0 + 1u], a0);
      b2 = blend_byte(src[idx0 + 2u], dst[idx0 + 2u], a0);
      out[idx0] = b0;
      out[idx0 + 1u] = b1;
      out[idx0 + 2u] = b2;
      out[idx0 + 3u] = 255u;
      h ^= b0; h *= prime;
      h ^= b1; h *= prime;
      h ^= b2; h *= prime;
      h ^= 255u; h *= prime;
      
      b0 = blend_byte(src[idx1], dst[idx1], a1);
      b1 = blend_byte(src[idx1 + 1u], dst[idx1 + 1u], a1);
      b2 = blend_byte(src[idx1 + 2u], dst[idx1 + 2u], a1);
      out[idx1] = b0;
      out[idx1 + 1u] = b1;
      out[idx1 + 2u] = b2;
      out[idx1 + 3u] = 255u;
      h ^= b0; h *= prime;
      h ^= b1; h *= prime;
      h ^= b2; h *= prime;
      h ^= 255u; h *= prime;
      
      b0 = blend_byte(src[idx2], dst[idx2], a2);
      b1 = blend_byte(src[idx2 + 1u], dst[idx2 + 1u], a2);
      b2 = blend_byte(src[idx2 + 2u], dst[idx2 + 2u], a2);
      out[idx2] = b0;
      out[idx2 + 1u] = b1;
      out[idx2 + 2u] = b2;
      out[idx2 + 3u] = 255u;
      h ^= b0; h *= prime;
      h ^= b1; h *= prime;
      h ^= b2; h *= prime;
      h ^= 255u; h *= prime;
      
      b0 = blend_byte(src[idx3], dst[idx3], a3);
      b1 = blend_byte(src[idx3 + 1u], dst[idx3 + 1u], a3);
      b2 = blend_byte(src[idx3 + 2u], dst[idx3 + 2u], a3);
      out[idx3] = b0;
      out[idx3 + 1u] = b1;
      out[idx3 + 2u] = b2;
      out[idx3 + 3u] = 255u;
      h ^= b0; h *= prime;
      h ^= b1; h *= prime;
      h ^= b2; h *= prime;
      h ^= 255u; h *= prime;
    }
    
    for (; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint8_t a = src[idx + 3u];
      uint8_t b0 = blend_byte(src[idx], dst[idx], a);
      uint8_t b1 = blend_byte(src[idx + 1u], dst[idx + 1u], a);
      uint8_t b2 = blend_byte(src[idx + 2u], dst[idx + 2u], a);
      out[idx] = b0;
      out[idx + 1u] = b1;
      out[idx + 2u] = b2;
      out[idx + 3u] = 255u;
      h ^= b0; h *= prime;
      h ^= b1; h *= prime;
      h ^= b2; h *= prime;
      h ^= 255u; h *= prime;
    }
    
    hash = h;
  }
  
  free(out);
  return hash;
}