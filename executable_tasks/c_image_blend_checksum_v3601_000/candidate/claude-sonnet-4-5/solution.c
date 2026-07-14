#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint32_t blend_pixel(uint32_t src_pixel, uint32_t dst_pixel, uint32_t alpha) {
  uint32_t rb_src = src_pixel & 0x00FF00FFu;
  uint32_t ag_src = (src_pixel >> 8) & 0x00FF00FFu;
  uint32_t rb_dst = dst_pixel & 0x00FF00FFu;
  uint32_t ag_dst = (dst_pixel >> 8) & 0x00FF00FFu;
  
  uint32_t inv_alpha = 255u - alpha;
  
  uint32_t rb_blend = ((rb_src * alpha + rb_dst * inv_alpha + 0x00800080u) >> 8) & 0x00FF00FFu;
  uint32_t ag_blend = ((ag_src * alpha + ag_dst * inv_alpha + 0x00800080u)) & 0xFF00FF00u;
  
  return rb_blend | ag_blend | 0xFF000000u;
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  size_t i = 0;
  
  for (; i + 7 < n; i += 8) {
    hash ^= (uint64_t)data[i];
    hash *= prime;
    hash ^= (uint64_t)data[i + 1];
    hash *= prime;
    hash ^= (uint64_t)data[i + 2];
    hash *= prime;
    hash ^= (uint64_t)data[i + 3];
    hash *= prime;
    hash ^= (uint64_t)data[i + 4];
    hash *= prime;
    hash ^= (uint64_t)data[i + 5];
    hash *= prime;
    hash ^= (uint64_t)data[i + 6];
    hash *= prime;
    hash ^= (uint64_t)data[i + 7];
    hash *= prime;
  }
  
  for (; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= prime;
  }
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 0;
  
  uint32_t* out = (uint32_t*)malloc(pixels * 4u);
  if (!out) return 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* src32 = (const uint32_t*)src;
    const uint32_t* dst32 = (const uint32_t*)dst;
    
    size_t i = 0;
    for (; i + 3 < pixels; i += 4) {
      uint32_t s0 = src32[i];
      uint32_t d0 = dst32[i];
      uint32_t a0 = (s0 >> 24) & 0xFFu;
      out[i] = blend_pixel(s0, d0, a0);
      
      uint32_t s1 = src32[i + 1];
      uint32_t d1 = dst32[i + 1];
      uint32_t a1 = (s1 >> 24) & 0xFFu;
      out[i + 1] = blend_pixel(s1, d1, a1);
      
      uint32_t s2 = src32[i + 2];
      uint32_t d2 = dst32[i + 2];
      uint32_t a2 = (s2 >> 24) & 0xFFu;
      out[i + 2] = blend_pixel(s2, d2, a2);
      
      uint32_t s3 = src32[i + 3];
      uint32_t d3 = dst32[i + 3];
      uint32_t a3 = (s3 >> 24) & 0xFFu;
      out[i + 3] = blend_pixel(s3, d3, a3);
    }
    
    for (; i < pixels; ++i) {
      uint32_t s = src32[i];
      uint32_t d = dst32[i];
      uint32_t a = (s >> 24) & 0xFFu;
      out[i] = blend_pixel(s, d, a);
    }
    
    hash = checksum_bytes((const uint8_t*)out, pixels * 4u);
  }
  
  free(out);
  return hash;
}