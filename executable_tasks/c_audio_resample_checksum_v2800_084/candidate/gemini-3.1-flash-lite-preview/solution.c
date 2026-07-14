#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint16_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  uint64_t hash = 0;
  // Use fixed-point arithmetic: 16.16 fixed point for interpolation
  // scale = (input_len - 1) / (output_len - 1)
  uint32_t step = (output_len > 1) ? (uint32_t)(((uint64_t)(input_len - 1) << 16) / (output_len - 1)) : 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      uint32_t pos = i * step;
      int left = pos >> 16;
      int frac = pos & 0xFFFF;
      
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;

      // Linear interpolation: src[left] * (1 - frac) + src[right] * frac
      // Using 16-bit fractional part
      int32_t val = (int32_t)src[left] * (65536 - frac) + (int32_t)src[right] * frac;
      out[i] = (int16_t)((val + 32768) >> 16);
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}