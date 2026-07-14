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
  const int32_t scale = (output_len > 1) ? ((input_len - 1) << 16) / (output_len - 1) : 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      int32_t pos_fixed = i * scale;
      int left = pos_fixed >> 16;
      int frac = pos_fixed & 0xFFFF;
      
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;

      int32_t s1 = src[left];
      int32_t s2 = src[right];
      
      // Linear interpolation: s1 + (s2 - s1) * frac / 65536
      int32_t val = (s1 << 16) + (s2 - s1) * frac;
      out[i] = (int16_t)((val + 32768) >> 16);
    }
    hash = checksum_i16(out, (size_t)output_len);
  }
  free(out);
  return hash;
}