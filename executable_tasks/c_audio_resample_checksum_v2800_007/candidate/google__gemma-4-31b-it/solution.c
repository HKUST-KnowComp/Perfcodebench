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
  
  // Precompute fixed-point step
  // We use a 32.32 fixed point representation for the position
  // step = (input_len - 1) / (output_len - 1)
  double step_d = (output_len > 1) ? (double)(input_len - 1) / (output_len - 1) : 0.0;
  uint64_t step_fixed = (uint64_t)(step_d * 4294967296.0);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t pos_fixed = 0;
    for (int i = 0; i < output_len; ++i) {
      uint32_t left = (uint32_t)(pos_fixed >> 32);
      uint32_t right = left + 1;
      if (right >= (uint32_t)input_len) right = (uint32_t)input_len - 1;
      
      uint32_t frac_fixed = (uint32_t)(pos_fixed & 0xFFFFFFFF);
      
      // Linear interpolation: src[left] + frac * (src[right] - src[left])
      // Using 64-bit to prevent overflow during multiplication
      int32_t diff = src[right] - src[left];
      int64_t sample_fixed = (int64_t)src[left] * 4294967296LL + (int64_t)diff * frac_fixed;
      
      // Rounding: add 0.5 (2^31) for positive, subtract 0.5 for negative
      int64_t rounded;
      if (sample_fixed >= 0) {
        rounded = (sample_fixed + 2147483648LL) >> 32;
      } else {
        rounded = (sample_fixed - 2147483648LL) >> 32;
      }
      
      out[i] = (int16_t)rounded;
      pos_fixed += step_fixed;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}