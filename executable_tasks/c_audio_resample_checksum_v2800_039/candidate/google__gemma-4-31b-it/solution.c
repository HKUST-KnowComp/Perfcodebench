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
  // pos = i * (input_len - 1) / (output_len - 1)
  // We use a 64-bit fixed point representation for the position.
  // Let's use 32 bits for the fractional part.
  const uint64_t FP_SHIFT = 32;
  const uint64_t FP_MASK = (1ULL << FP_SHIFT) - 1;
  const uint64_t FP_HALF = (1ULL << (FP_SHIFT - 1));

  uint64_t step_fp = 0;
  if (output_len > 1) {
    step_fp = ((uint64_t)(input_len - 1) << FP_SHIFT) / (output_len - 1);
  } else {
    step_fp = 0;
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_pos_fp = 0;
    for (int i = 0; i < output_len; ++i) {
      int left = (int)(current_pos_fp >> FP_SHIFT);
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      
      uint64_t frac_fp = current_pos_fp & FP_MASK;
      
      // Linear interpolation: src[left] * (1 - frac) + src[right] * frac
      // = src[left] + frac * (src[right] - src[left])
      int32_t diff = src[right] - src[left];
      int64_t sample_fp = ((int64_t)src[left] << FP_SHIFT) + 
                          (((int64_t)diff * (int64_t)frac_fp));
      
      // Rounding: (sample_fp + 0.5) if positive, (sample_fp - 0.5) if negative
      // In fixed point, 0.5 is FP_HALF
      int16_t result;
      if (sample_fp >= 0) {
        result = (int16_t)((sample_fp + FP_HALF) >> FP_SHIFT);
      } else {
        result = (int16_t)((sample_fp - FP_HALF) >> FP_SHIFT);
      }
      out[i] = result;
      current_pos_fp += step_fp;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}