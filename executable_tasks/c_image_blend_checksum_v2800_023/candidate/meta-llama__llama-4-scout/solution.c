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
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* out = (uint8_t*)malloc(pixels * 4u);
    if (!out) return 0;
    for (size_t i = 0; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint8_t a = src[idx + 3u];
      uint32_t sa = (uint32_t)src[idx];
      uint32_t da = (uint32_t)dst[idx];
      uint32_t sb = (uint32_t)src[idx + 1u];
      uint32_t db = (uint32_t)dst[idx + 1u];
      uint32_t sg = (uint32_t)src[idx + 2u];
      uint32_t dg = (uint32_t)dst[idx + 2u];
      out[idx] = (uint8_t)(((sa * a + da * (255u - a)) / 255u) & 0xFF);
      out[idx + 1u] = (uint8_t)(((sb * a + db * (255u - a)) / 255u) & 0xFF);
      out[idx + 2u] = (uint8_t)(((sg * a + dg * (255u - a)) / 255u) & 0xFF);
      out[idx + 3u] = 255u;
    }
    hash = checksum_bytes(out, pixels * 4u);
    free(out);
  }
  return hash;
}
