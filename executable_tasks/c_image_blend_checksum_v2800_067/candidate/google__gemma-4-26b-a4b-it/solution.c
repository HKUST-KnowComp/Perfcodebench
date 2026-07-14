#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

// The original formula: (s * a + d * (255 - a) + 127) / 255
// To maintain exact correctness, we must use the same integer arithmetic.
// We can optimize the loop by processing pixels in chunks and avoiding repeated indexing.

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t num_bytes = (size_t)width * (size_t)height * 4u;
  uint8_t* out = (uint8_t*)malloc(num_bytes);
  if (!out) return 0;

  uint64_t last_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < num_bytes; i += 4) {
      uint32_t a = src[i + 3];
      uint32_t inv_a = 255u - a;
      
      // Exact reproduction of the original blend_byte logic
      out[i]     = (uint8_t)(((uint32_t)src[i]     * a + (uint32_t)dst[i]     * inv_a + 127u) / 255u);
      out[i + 1] = (uint8_t)(((uint32_t)src[i + 1] * a + (uint32_t)dst[i + 1] * inv_a + 127u) / 255u);
      out[i + 2] = (uint8_t)(((uint32_t)src[i + 2] * a + (uint32_t)dst[i + 2] * inv_a + 127u) / 255u);
      out[i + 3] = 255u;
    }
    last_hash = checksum_bytes(out, num_bytes);
  }

  free(out);
  return last_hash;
}