#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return (uint8_t)(((uint32_t)s * (uint32_t)a + (uint32_t)d * (uint32_t)(255u - a) + 127u) / 255u);
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
  size_t pixels = (size_t)width * (size_t)height;
  uint8_t* out = (uint8_t*)malloc(pixels * 4u);
  if (!out) return 0;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* src_ptr = src;
    const uint8_t* dst_ptr = dst;
    uint8_t* out_ptr = out;
    for (size_t i = 0; i < pixels; ++i) {
      uint8_t a = src_ptr[3];
      out_ptr[0] = blend_byte(src_ptr[0], dst_ptr[0], a);
      out_ptr[1] = blend_byte(src_ptr[1], dst_ptr[1], a);
      out_ptr[2] = blend_byte(src_ptr[2], dst_ptr[2], a);
      out_ptr[3] = 255u;
      src_ptr += 4;
      dst_ptr += 4;
      out_ptr += 4;
    }
    hash = checksum_bytes(out, pixels * 4u);
  }
  free(out);
  return hash;
}
