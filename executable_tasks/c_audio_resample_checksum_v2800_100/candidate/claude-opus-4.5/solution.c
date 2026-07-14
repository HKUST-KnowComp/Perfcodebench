#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  size_t i = 0;
  size_t n4 = n & ~(size_t)3;
  for (; i < n4; i += 4) {
    hash ^= (uint16_t)data[i];
    hash *= prime;
    hash ^= (uint16_t)data[i + 1];
    hash *= prime;
    hash ^= (uint16_t)data[i + 2];
    hash *= prime;
    hash ^= (uint16_t)data[i + 3];
    hash *= prime;
  }
  for (; i < n; ++i) {
    hash ^= (uint16_t)data[i];
    hash *= prime;
  }
  return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  if (output_len <= 0 || input_len <= 0) return 0;
  
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;
  
  uint64_t hash = 0;
  
  /* Fixed-point arithmetic: 32 bits for fractional part */
  const uint64_t frac_bits = 32;
  const uint64_t frac_one = (uint64_t)1 << frac_bits;
  const uint64_t frac_half = frac_one >> 1;
  
  /* step = (input_len - 1) / (output_len - 1) in fixed-point */
  uint64_t step = 0;
  if (output_len > 1) {
    step = ((uint64_t)(input_len - 1) << frac_bits) / (uint64_t)(output_len - 1);
  }
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t pos_fixed = 0;
    
    for (int i = 0; i < output_len; ++i) {
      int left = (int)(pos_fixed >> frac_bits);
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      
      /* frac is the fractional part, scaled to [0, frac_one) */
      uint64_t frac = pos_fixed & (frac_one - 1);
      
      /* Linear interpolation in fixed-point:
       * sample = src[left] * (1 - frac) + src[right] * frac
       * Using 64-bit arithmetic to avoid overflow */
      int32_t left_val = src[left];
      int32_t right_val = src[right];
      
      /* Compute: left_val * (frac_one - frac) + right_val * frac
       * Then divide by frac_one with rounding */
      int64_t interp = (int64_t)left_val * (int64_t)(frac_one - frac) 
                     + (int64_t)right_val * (int64_t)frac;
      
      /* Round to nearest: add half before truncating */
      int16_t sample;
      if (interp >= 0) {
        sample = (int16_t)((interp + (int64_t)frac_half) >> frac_bits);
      } else {
        sample = (int16_t)((interp - (int64_t)frac_half) >> frac_bits);
      }
      
      out[i] = sample;
      pos_fixed += step;
    }
    
    hash = checksum_i16(out, (size_t)output_len);
  }
  
  free(out);
  return hash;
}