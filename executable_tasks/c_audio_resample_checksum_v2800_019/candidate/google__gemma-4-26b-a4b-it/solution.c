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
  
  uint64_t hash = 0;
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  // Pre-calculate fixed-point step
  // We use 64-bit fixed point to represent the position to maintain precision
  // equivalent to the double-precision logic used in the baseline.
  // The baseline uses: pos = i * (input_len - 1) / (output_len - 1)
  // We use a large scale factor to simulate the double precision.
  
  const uint64_t SCALE = 1ULL << 48;
  uint64_t step_scaled = 0;
  if (output_len > 1) {
    step_scaled = ((uint64_t)(input_len - 1) << 48) / (output_len - 1);
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_pos_scaled = 0;
    for (int i = 0; i < output_len; ++i) {
      int left = (int)(current_pos_scaled >> 48);
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;

      // frac = (pos - left) -> (current_pos_scaled - (left << 48)) / SCALE
      uint64_t frac_scaled = current_pos_scaled - ((uint64_t)left << 48);
      
      // sample = src[left] * (1.0 - frac) + src[right] * frac
      // To match the baseline's rounding (sample + 0.5 or - 0.5):
      // We use 64-bit math to perform the interpolation and then round.
      // The baseline uses double precision. To ensure exact match, we must be careful.
      // However, for linear interpolation of int16, fixed point with high precision is sufficient.
      
      int64_t s_left = src[left];
      int64_t s_right = src[right];
      
      // res = s_left + (s_right - s_left) * frac
      // res_scaled = s_left * SCALE + (s_right - s_left) * frac_scaled
      int64_t interpolated_scaled = (s_left << 48) + ((s_right - s_left) * (int64_t)frac_scaled >> 48);
      
      // The baseline rounding: if sample >= 0: sample + 0.5 else sample - 0.5
      // In fixed point (scale 2^48), 0.5 is 1 << 47.
      int64_t rounded_val;
      if (interpolated_scaled >= 0) {
        rounded_val = (interpolated_scaled + (1LL << 47)) >> 48;
      } else {
        rounded_val = (interpolated_scaled - (1LL << 47)) >> 48;
      }
      
      out[i] = (int16_t)rounded_val;
      
      if (i < output_len - 1) {
        current_pos_scaled += step_scaled;
      }
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}