#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* restrict s = src;
    const uint8_t* restrict d = dst;
    uint64_t h = 1469598103934665603ULL; // FNV-1a offset basis
    for (size_t i = 0; i < pixels; ++i) {
      uint32_t a = (uint32_t)s[3];
      uint32_t inva = 255u - a;

      uint32_t t0 = (uint32_t)s[0] * a + (uint32_t)d[0] * inva + 127u;
      uint8_t out0 = (uint8_t)(t0 / 255u);
      h ^= (uint64_t)out0;
      h *= 1099511628211ULL;

      uint32_t t1 = (uint32_t)s[1] * a + (uint32_t)d[1] * inva + 127u;
      uint8_t out1 = (uint8_t)(t1 / 255u);
      h ^= (uint64_t)out1;
      h *= 1099511628211ULL;

      uint32_t t2 = (uint32_t)s[2] * a + (uint32_t)d[2] * inva + 127u;
      uint8_t out2 = (uint8_t)(t2 / 255u);
      h ^= (uint64_t)out2;
      h *= 1099511628211ULL;

      // Alpha channel is forced to 255 in output
      h ^= 255u;
      h *= 1099511628211ULL;

      s += 4;
      d += 4;
    }
    hash = h; // only the last iteration's checksum is returned
  }
  return hash;
}
