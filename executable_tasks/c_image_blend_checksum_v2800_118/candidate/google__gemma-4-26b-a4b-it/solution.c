#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

// Optimized blend using fast integer approximation for (x + 127) / 255
// (x + 1 + (x >> 8)) >> 8 is a common trick for division by 255
static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = (uint32_t)s * a + (uint32_t)d * (255u - a) + 127u;
    return (uint8_t)((val + (val >> 8)) >> 8);
}

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
      size_t idx = i << 2; // i * 4
      uint8_t a = src[idx + 3u];
      
      // Manual inlining of blend_byte to assist compiler vectorization
      uint32_t base_a = (uint32_t)a;
      uint32_t inv_a = 255u - base_a;

      uint32_t r_val = (uint32_t)src[idx] * base_a + (uint32_t)dst[idx] * inv_a + 127u;
      out[idx] = (uint8_t)((r_val + (r_val >> 8)) >> 8);

      uint32_t g_val = (uint32_t)src[idx + 1u] * base_a + (uint32_t)dst[idx + 1u] * inv_a + 127u;
      out[idx + 1u] = (uint8_t)((g_val + (g_val >> 8)) >> 8);

      uint32_t b_val = (uint32_t)src[idx + 2u] * base_a + (uint32_t)dst[idx + 2u] * inv_a + 127u;
      out[idx + 2u] = (uint8_t)((b_val + (b_val >> 8)) >> 8);

      out[idx + 3u] = 255u;
    }
    hash = checksum_bytes(out, total_bytes);
  }

  free(out);
  return hash;
}