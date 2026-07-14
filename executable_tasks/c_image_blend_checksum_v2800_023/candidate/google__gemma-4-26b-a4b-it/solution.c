#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/* 
 * The original blend function: 
 * (s * a + d * (255 - a) + 127) / 255 
 * 
 * We can rewrite this to minimize operations. 
 * Let's use the fast approximation for division by 255: 
 * x / 255 ≈ ((x + 1 + (x >> 8)) >> 8) or more accurately for 32-bit: 
 * ((x * 0x01010101ULL) >> 32) is not quite right for 255. 
 * A common fast way for (val / 255) is ((val + 1 + (val >> 8)) >> 8).
 * However, to ensure EXACT correctness as per the requirement, 
 * we must be careful. The requirement says 'Returned checksum must exactly match'.
 * This means we cannot use an approximation if the original used integer division.
 * 
 * Let's re-examine the original: 
 * (uint32_t)s * (uint32_t)a + (uint32_t)d * (uint32_t)(255u - a) + 127u) / 255u
 * 
 * We can optimize the math: 
 * s*a + d*255 - d*a + 127 / 255
 * = (a*(s-d) + d*255 + 127) / 255
 * = d + (a*(s-d) + 127) / 255
 * 
 * Let's use the optimized math and move malloc out of the loop.
 */

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    int32_t diff = (int32_t)s - (int32_t)d;
    return (uint8_t)((diff * (int32_t)a + 127 + (int32_t)d * 255) / 255);
}

/* To ensure exact correctness, we must use the exact formula provided. 
 * The bottleneck is the division. 
 * Since we need EXACT correctness, we will use the original formula 
 * but optimize the loop structure and memory.
 */

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  (void)width;
  (void)height;
  size_t pixels = (size_t)width * (size_t)height;
  size_t total_bytes = pixels * 4u;
  uint64_t hash = 0;
  
  uint8_t* out = (uint8_t*)malloc(total_bytes);
  if (!out) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < pixels; ++i) {
      size_t idx = i << 2;
      uint32_t a = src[idx + 3u];
      uint32_t inv_a = 255u - a;
      
      // Exact implementation of the original formula to guarantee checksum match
      out[idx]     = (uint8_t)(((uint32_t)src[idx]     * a + (uint32_t)dst[idx]     * inv_a + 127u) / 255u);
      out[idx + 1u] = (uint8_t)(((uint32_t)src[idx + 1u] * a + (uint32_t)dst[idx + 1u] * inv_a + 127u) / 255u);
      out[idx + 2u] = (uint8_t)(((uint32_t)src[idx + 2u] * a + (uint32_t)dst[idx + 2u] * inv_a + 127u) / 255u);
      out[idx + 3u] = 255u;
    }
    hash = checksum_bytes(out, total_bytes);
  }
  
  free(out);
  return hash;
}