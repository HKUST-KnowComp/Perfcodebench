#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  if (iters <= 0)
    return 0;

  size_t pixels = (size_t)width * (size_t)height;
  uint8_t* out = (uint8_t*)malloc(pixels * 4u);
  if (!out)
    return 0;

  for (size_t i = 0; i < pixels; ++i) {
    size_t idx = i * 4u;
    uint8_t a = src[idx + 3u];
    uint32_t inv_a = 255u - a;
    out[idx]     = (uint8_t)(((uint32_t)src[idx]     * a + (uint32_t)dst[idx]     * inv_a + 127u) / 255u);
    out[idx + 1u] = (uint8_t)(((uint32_t)src[idx + 1u] * a + (uint32_t)dst[idx + 1u] * inv_a + 127u) / 255u);
    out[idx + 2u] = (uint8_t)(((uint32_t)src[idx + 2u] * a + (uint32_t)dst[idx + 2u] * inv_a + 127u) / 255u);
    out[idx + 3u] = 255u;
  }

  uint64_t hash = checksum_bytes(out, pixels * 4u);
  free(out);
  return hash;
}
