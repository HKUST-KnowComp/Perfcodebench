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
  (void)width;
  (void)height;
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 0;
  uint8_t* out = (uint8_t*)malloc(pixels * 4u);
  if (!out) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint8_t* src_pixel = src + idx;
      uint8_t* dst_pixel = dst + idx;
      uint8_t* out_pixel = out + idx;

      uint8_t a = src_pixel[3];
      uint32_t a_val = a;
      uint32_t inv_a = 255 - a_val;

      out_pixel[0] = (src_pixel[0] * a_val + dst_pixel[0] * inv_a + 127) / 255;
      out_pixel[1] = (src_pixel[1] * a_val + dst_pixel[1] * inv_a + 127) / 255;
      out_pixel[2] = (src_pixel[2] * a_val + dst_pixel[2] * inv_a + 127) / 255;
      out_pixel[3] = 255;
    }
    hash = checksum_bytes(out, pixels * 4u);
  }

  free(out);
  return hash;
}