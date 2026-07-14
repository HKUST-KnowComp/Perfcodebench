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
  const int64_t FRAC_BITS = 32;
  const int64_t FRAC_ONE = (int64_t)1 << FRAC_BITS;
  const int64_t FRAC_HALF = FRAC_ONE >> 1;
  
  /* step = (input_len - 1) / (output_len - 1) in fixed-point */
  int64_t step = 0;
  if (output_len > 1) {
    step = ((int64_t)(input_len - 1) << FRAC_BITS) / (output_len - 1);
  }
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t pos_fixed = 0;
    
    for (int i = 0; i < output_len; ++i) {
      int left = (int)(pos_fixed >> FRAC_BITS);
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      
      int64_t frac = pos_fixed & (FRAC_ONE - 1);
      int64_t one_minus_frac = FRAC_ONE - frac;
      
      /* Linear interpolation in fixed-point */
      int64_t sample_fixed = (int64_t)src[left] * one_minus_frac + (int64_t)src[right] * frac;
      
      /* Round to nearest: add 0.5 (FRAC_HALF) before shifting for positive,
         subtract 0.5 for negative */
      int16_t sample;
      if (sample_fixed >= 0) {
        sample = (int16_t)((sample_fixed + FRAC_HALF) >> FRAC_BITS);
      } else {
        sample = (int16_t)((sample_fixed - FRAC_HALF) >> FRAC_BITS);
      }
      out[i] = sample;
      
      pos_fixed += step;
    }
    
    hash = checksum_i16(out, (size_t)output_len);
  }
  
  free(out);
  return hash;
}