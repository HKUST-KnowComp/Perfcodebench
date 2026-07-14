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

static uint64_t checksum_bytes_fast(const uint8_t* data, size_t n) {
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
  uint8_t* out = (uint8_t*)malloc(pixels * 4u);
  if (!out) return 0;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0;
    
    for (; i + 3 < pixels; i += 4) {
      size_t idx0 = i * 4u;
      size_t idx1 = idx0 + 4u;
      size_t idx2 = idx0 + 8u;
      size_t idx3 = idx0 + 12u;
      
      uint32_t src0, src1, src2, src3;
      uint32_t dst0, dst1, dst2, dst3;
      
      __builtin_memcpy(&src0, &src[idx0], 4);
      __builtin_memcpy(&src1, &src[idx1], 4);
      __builtin_memcpy(&src2, &src[idx2], 4);
      __builtin_memcpy(&src3, &src[idx3], 4);
      
      __builtin_memcpy(&dst0, &dst[idx0], 4);
      __builtin_memcpy(&dst1, &dst[idx1], 4);
      __builtin_memcpy(&dst2, &dst[idx2], 4);
      __builtin_memcpy(&dst3, &dst[idx3], 4);
      
      uint32_t alpha0 = (src0 >> 24) & 0xFFu;
      uint32_t alpha1 = (src1 >> 24) & 0xFFu;
      uint32_t alpha2 = (src2 >> 24) & 0xFFu;
      uint32_t alpha3 = (src3 >> 24) & 0xFFu;
      
      uint32_t out0 = blend_pixel(src0, dst0, alpha0);
      uint32_t out1 = blend_pixel(src1, dst1, alpha1);
      uint32_t out2 = blend_pixel(src2, dst2, alpha2);
      uint32_t out3 = blend_pixel(src3, dst3, alpha3);
      
      __builtin_memcpy(&out[idx0], &out0, 4);
      __builtin_memcpy(&out[idx1], &out1, 4);
      __builtin_memcpy(&out[idx2], &out2, 4);
      __builtin_memcpy(&out[idx3], &out3, 4);
    }
    
    for (; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint32_t src_pixel, dst_pixel;
      __builtin_memcpy(&src_pixel, &src[idx], 4);
      __builtin_memcpy(&dst_pixel, &dst[idx], 4);
      
      uint32_t alpha = (src_pixel >> 24) & 0xFFu;
      uint32_t out_pixel = blend_pixel(src_pixel, dst_pixel, alpha);
      
      __builtin_memcpy(&out[idx], &out_pixel, 4);
    }
    
    hash = checksum_bytes_fast(out, pixels * 4u);
  }
  
  free(out);
  return hash;
}