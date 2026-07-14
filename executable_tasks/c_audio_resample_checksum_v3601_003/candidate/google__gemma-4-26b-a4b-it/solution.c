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
  
  // Pre-calculate fixed-point step
  // We use 64-bit fixed point to mimic double precision behavior for the fractional part
  // The original formula: pos = i * (input_len - 1) / (output_len - 1)
  // We use a large scale to represent the fractional part accurately.
  // To match the exact rounding behavior of (sample + 0.5) or (sample - 0.5),
  // we use 1L << 32 as our fractional scale.
  
  const uint64_t SCALE = 1ULL << 32;
  uint64_t step_num = (uint64_t)(input_len - 1) << 32;
  uint64_t step_den = (uint64_t)(output_len - 1);

  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      // pos_fixed = i * (input_len - 1) / (output_len - 1) in SCALE units
      // To avoid overflow and maintain precision, we calculate pos_fixed carefully
      uint64_t pos_fixed = ((uint64_t)i * step_num) / step_den;
      
      int left = (int)(pos_fixed >> 32);
      uint32_t frac_part = (uint32_t)(pos_fixed & 0xFFFFFFFFULL);
      
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;

      // Linear interpolation: src[left] * (1 - frac) + src[right] * frac
      // frac = frac_part / SCALE
      // sample = (src[left] * (SCALE - frac_part) + src[right] * frac_part) / SCALE
      
      int64_t s_left = src[left];
      int64_t s_right = src[right];
      
      // We need to match the rounding: (sample + 0.5) if sample >= 0 else (sample - 0.5)
      // This is equivalent to floor(sample + 0.5) for positive and ceil(sample - 0.5) for negative.
      // In integer math: (val + offset) / SCALE
      // To match the double precision rounding exactly:
      // sample = (s_left * (SCALE - frac_part) + s_right * frac_part) / SCALE
      // We add 0.5 * SCALE to the numerator before dividing by SCALE to handle rounding.
      
      int64_t numerator = s_left * (int64_t)(SCALE - frac_part) + s_right * (int64_t)frac_part;
      
      int16_t res;
      if (numerator >= 0) {
        res = (int16_t)((numerator + (1LL << 31)) >> 32);
      } else {
        res = (int16_t)((numerator - (1LL << 31)) >> 32);
      }
      out[i] = res;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}