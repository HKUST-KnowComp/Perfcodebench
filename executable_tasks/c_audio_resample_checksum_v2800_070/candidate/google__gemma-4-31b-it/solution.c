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
  const uint64_t scale = 1ULL << 32;
  const uint64_t step = (output_len > 1) ? 
      ((uint64_t)(input_len - 1) * scale) / (output_len - 1) : 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_pos_fp = 0;
    for (int i = 0; i < output_len; ++i) {
      uint64_t left = current_pos_fp >> 32;
      uint64_t frac_fp = current_pos_fp & 0xFFFFFFFF;
      
      uint64_t right = left + 1;
      if (right >= (uint64_t)input_len) right = (uint64_t)input_len - 1;

      int32_t s_left = src[left];
      int32_t s_right = src[right];
      
      // Linear interpolation: sample = s_left + frac * (s_right - s_left)
      // Using fixed point: sample_fp = s_left * scale + (frac_fp * (s_right - s_left))
      int64_t sample_fp = ((int64_t)s_left << 32) + ((int64_t)frac_fp * (s_right - s_left));
      
      // Rounding: (sample_fp + 0.5 * scale) if positive, (sample_fp - 0.5 * scale) if negative
      // 0.5 * scale is 1ULL << 31
      if (sample_fp >= 0) {
        out[i] = (int16_t)((sample_fp + (1ULL << 31)) >> 32);
      } else {
        out[i] = (int16_t)((sample_fp - (1ULL << 31)) >> 32);
      }
      
      current_pos_fp += step;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}