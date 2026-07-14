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
  if (input_len <= 1) {
    // Handle edge case where input_len - 1 would be 0
    // The original code would result in NaN/Inf due to division by zero if output_len > 1
    // However, following the original logic's structure:
    int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;
    for (int i = 0; i < output_len; ++i) out[i] = src[0];
    uint64_t hash = checksum_i16(out, (size_t)output_len);
    free(out);
    return hash;
  }

  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  uint64_t hash = 0;
  
  // Pre-calculate fixed-point step
  // pos = i * (input_len - 1) / (output_len - 1)
  // We use a large scale for fixed-point precision to match double behavior
  // 1.0 in double is equivalent to (1LL << 48) in our fixed point
  const uint64_t SCALE = 1ULL << 48;
  const uint64_t step_scaled = ((uint64_t)(input_len - 1) << 48) / (uint64_t)(output_len - 1);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_pos_scaled = 0;
    for (int i = 0; i < output_len; ++i) {
      // current_pos_scaled represents (pos * SCALE)
      int left = (int)(current_pos_scaled >> 48);
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;

      // frac = pos - left. In fixed point: (current_pos_scaled % SCALE) / SCALE
      // We need to perform: sample = src[left]*(1-frac) + src[right]*frac
      // To match the original (sample + 0.5) rounding logic exactly:
      // The original uses double precision. To mimic (double)src[left] * (1.0 - frac) + (double)src[right] * frac
      // with rounding, we use the fractional part scaled.
      
      uint64_t frac_scaled = current_pos_scaled & (SCALE - 1);
      
      // We use 128-bit intermediate or careful 64-bit to avoid overflow
      // sample_scaled = src[left] * (SCALE - frac_scaled) + src[right] * frac_scaled
      // To handle signedness and rounding (+0.5 or -0.5):
      // The original code: if (sample >= 0) out = sample + 0.5 else out = sample - 0.5
      // This is equivalent to round(sample) for positive and round(sample) for negative
      // but specifically it's 'round half away from zero'.
      
      int64_t s_left = src[left];
      int64_t s_right = src[right];
      
      // Using __int128 for the intermediate high-precision fixed-point calculation
      // to ensure we match the double precision rounding exactly.
      __int128 sample_fixed = (__int128)s_left * (SCALE - frac_scaled) + (__int128)s_right * frac_scaled;
      // sample_fixed is now (sample * SCALE)
      
      // To get the integer part with rounding:
      // We need (sample_fixed / SCALE) with rounding logic.
      // Original: if (sample >= 0) out = (int16_t)(sample + 0.5) else out = (int16_t)(sample - 0.5)
      // This is equivalent to: 
      // if (sample_fixed >= 0) out = (sample_fixed + SCALE/2) / SCALE
      // if (sample_fixed < 0)  out = (sample_fixed - SCALE/2) / SCALE
      
      int64_t res;
      if (sample_fixed >= 0) {
        res = (int64_t)((sample_fixed + (SCALE >> 1)) >> 48);
      } else {
        res = (int64_t)((sample_fixed - (SCALE >> 1)) >> 48);
      }
      
      out[i] = (int16_t)res;
      
      if (i < output_len - 1) {
        current_pos_scaled += step_scaled;
      }
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}