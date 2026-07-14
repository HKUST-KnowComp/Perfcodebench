#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  if (output_len <= 0 || input_len <= 0) return 0;
  
  uint64_t hash = 0;
  
  /* Use fixed-point arithmetic with 32-bit fractional precision */
  const uint64_t FRAC_BITS = 32;
  const uint64_t FRAC_ONE = 1ULL << FRAC_BITS;
  const uint64_t FRAC_HALF = 1ULL << (FRAC_BITS - 1);
  
  /* Calculate step in fixed-point */
  uint64_t step;
  if (output_len == 1) {
    step = 0;
  } else {
    step = ((uint64_t)(input_len - 1) << FRAC_BITS) / (uint64_t)(output_len - 1);
  }
  
  const int last_idx = input_len - 1;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    uint64_t pos_fixed = 0;
    
    for (int i = 0; i < output_len; ++i) {
      int left = (int)(pos_fixed >> FRAC_BITS);
      uint64_t frac = pos_fixed & (FRAC_ONE - 1);
      
      int right = left + 1;
      if (right > last_idx) right = last_idx;
      
      int32_t s_left = src[left];
      int32_t s_right = src[right];
      
      /* Linear interpolation in fixed-point:
       * sample = s_left * (1 - frac) + s_right * frac
       * Using 64-bit intermediate to avoid overflow */
      int64_t interp = (int64_t)s_left * (int64_t)(FRAC_ONE - frac) + 
                       (int64_t)s_right * (int64_t)frac;
      
      /* Round to nearest: add 0.5 (FRAC_HALF) for positive, subtract for negative */
      int16_t sample;
      if (interp >= 0) {
        sample = (int16_t)((interp + FRAC_HALF) >> FRAC_BITS);
      } else {
        sample = (int16_t)((interp - FRAC_HALF) >> FRAC_BITS);
      }
      
      /* Inline checksum computation */
      hash ^= (uint16_t)sample;
      hash *= 1099511628211ULL;
      
      pos_fixed += step;
    }
  }
  
  return hash;
}