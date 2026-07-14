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
  
  /* Use 32-bit fixed-point for fractional part (high precision) */
  const uint64_t FRAC_BITS = 32;
  const uint64_t FRAC_ONE = 1ULL << FRAC_BITS;
  const uint64_t FRAC_HALF = 1ULL << (FRAC_BITS - 1);
  
  /* step = (input_len - 1) / (output_len - 1) in fixed-point */
  uint64_t step = 0;
  if (output_len > 1) {
    step = ((uint64_t)(input_len - 1) << FRAC_BITS) / (uint64_t)(output_len - 1);
  }
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t pos_fixed = 0;
    
    for (int i = 0; i < output_len; ++i) {
      uint32_t left = (uint32_t)(pos_fixed >> FRAC_BITS);
      uint32_t right = left + 1;
      if (right >= (uint32_t)input_len) right = (uint32_t)(input_len - 1);
      
      uint64_t frac = pos_fixed & (FRAC_ONE - 1);
      
      int32_t s_left = src[left];
      int32_t s_right = src[right];
      
      /* Linear interpolation: sample = s_left * (1 - frac) + s_right * frac */
      /* = s_left + frac * (s_right - s_left) */
      int64_t diff = s_right - s_left;
      int64_t sample_fixed = ((int64_t)s_left << FRAC_BITS) + diff * (int64_t)frac;
      
      /* Round to nearest: add 0.5 (FRAC_HALF) for positive, subtract for negative */
      int16_t sample;
      if (sample_fixed >= 0) {
        sample = (int16_t)((sample_fixed + (int64_t)FRAC_HALF) >> FRAC_BITS);
      } else {
        sample = (int16_t)((sample_fixed - (int64_t)FRAC_HALF) >> FRAC_BITS);
      }
      
      out[i] = sample;
      pos_fixed += step;
    }
    
    hash = checksum_i16(out, (size_t)output_len);
  }
  
  free(out);
  return hash;
}