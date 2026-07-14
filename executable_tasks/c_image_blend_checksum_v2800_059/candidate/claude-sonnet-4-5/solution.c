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
  uint64_t hash0 = 1469598103934665603ULL;
  uint64_t hash1 = 1469598103934665603ULL;
  uint64_t hash2 = 1469598103934665603ULL;
  uint64_t hash3 = 1469598103934665603ULL;
  uint64_t hash4 = 1469598103934665603ULL;
  uint64_t hash5 = 1469598103934665603ULL;
  uint64_t hash6 = 1469598103934665603ULL;
  uint64_t hash7 = 1469598103934665603ULL;
  
  const uint64_t prime = 1099511628211ULL;
  size_t i = 0;
  size_t limit = n - (n % 8);
  
  for (; i < limit; i += 8) {
    hash0 ^= (uint64_t)data[i];
    hash0 *= prime;
    hash1 ^= (uint64_t)data[i + 1];
    hash1 *= prime;
    hash2 ^= (uint64_t)data[i + 2];
    hash2 *= prime;
    hash3 ^= (uint64_t)data[i + 3];
    hash3 *= prime;
    hash4 ^= (uint64_t)data[i + 4];
    hash4 *= prime;
    hash5 ^= (uint64_t)data[i + 5];
    hash5 *= prime;
    hash6 ^= (uint64_t)data[i + 6];
    hash6 *= prime;
    hash7 ^= (uint64_t)data[i + 7];
    hash7 *= prime;
  }
  
  uint64_t hash = hash0 ^ hash1 ^ hash2 ^ hash3 ^ hash4 ^ hash5 ^ hash6 ^ hash7;
  
  for (; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= prime;
  }
  
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  uint8_t* out = (uint8_t*)malloc(pixels * 4u);
  if (!out) return 0;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* src32 = (const uint32_t*)src;
    const uint32_t* dst32 = (const uint32_t*)dst;
    uint32_t* out32 = (uint32_t*)out;
    
    for (size_t i = 0; i < pixels; ++i) {
      uint32_t src_pixel = src32[i];
      uint32_t alpha = (src_pixel >> 24) & 0xFFu;
      out32[i] = blend_pixel(src_pixel, dst32[i], alpha);
    }
    
    hash = checksum_bytes(out, pixels * 4u);
  }
  
  free(out);
  return hash;
}