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
  if (output_len <= 0) return 0;
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  uint64_t hash = 0;
  // Use fixed-point arithmetic: scale = (input_len - 1) / (output_len - 1)
  // We use 32-bit fractional part for precision
  uint64_t step = (output_len > 1) ? (((uint64_t)(input_len - 1) << 32) / (output_len - 1)) : 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      uint64_t pos = (uint64_t)i * step;
      int left = (int)(pos >> 32);
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      
      uint32_t frac = (uint32_t)pos;
      int32_t s1 = src[left];
      int32_t s2 = src[right];
      
      // Linear interpolation: s1 + (s2 - s1) * frac
      // Using 32-bit fixed point: result = s1 + ((s2 - s1) * frac >> 32)
      int32_t sample = s1 + (int32_t)(((int64_t)(s2 - s1) * frac) >> 32);
      out[i] = (int16_t)sample;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }
  free(out);
  return hash;
}