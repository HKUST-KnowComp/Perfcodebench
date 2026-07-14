#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  size_t i = 0;
  for (; i + 4 <= n; i += 4) {
    hash ^= (uint16_t)data[i];
    hash *= 1099511628211ULL;
    hash ^= (uint16_t)data[i + 1];
    hash *= 1099511628211ULL;
    hash ^= (uint16_t)data[i + 2];
    hash *= 1099511628211ULL;
    hash ^= (uint16_t)data[i + 3];
    hash *= 1099511628211ULL;
  }
  for (; i < n; ++i) {
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
  
  // Use 32-bit fixed point for fractional part (high precision)
  // scale = (input_len - 1) / (output_len - 1) in fixed point
  const uint64_t FRAC_BITS = 32;
  const uint64_t FRAC_ONE = 1ULL << FRAC_BITS;
  const uint64_t FRAC_HALF = 1ULL << (FRAC_BITS - 1);
  
  uint64_t step = 0;
  if (output_len > 1) {
    step = ((uint64_t)(input_len - 1) << FRAC_BITS) / (uint64_t)(output_len - 1);
  }
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t pos_fixed = 0;
    int i = 0;
    
    // Unroll by 4
    int limit = output_len - 3;
    for (; i < limit; i += 4) {
      // Sample 0
      {
        int left = (int)(pos_fixed >> FRAC_BITS);
        uint64_t frac = pos_fixed & (FRAC_ONE - 1);
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;
        int32_t s_left = src[left];
        int32_t s_right = src[right];
        int64_t sample_fixed = ((int64_t)s_left << FRAC_BITS) + (int64_t)(s_right - s_left) * (int64_t)frac;
        // Round: add 0.5 in fixed point, then shift
        if (sample_fixed >= 0) {
          out[i] = (int16_t)((sample_fixed + (int64_t)FRAC_HALF) >> FRAC_BITS);
        } else {
          out[i] = (int16_t)((sample_fixed - (int64_t)FRAC_HALF) >> FRAC_BITS);
        }
        pos_fixed += step;
      }
      // Sample 1
      {
        int left = (int)(pos_fixed >> FRAC_BITS);
        uint64_t frac = pos_fixed & (FRAC_ONE - 1);
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;
        int32_t s_left = src[left];
        int32_t s_right = src[right];
        int64_t sample_fixed = ((int64_t)s_left << FRAC_BITS) + (int64_t)(s_right - s_left) * (int64_t)frac;
        if (sample_fixed >= 0) {
          out[i + 1] = (int16_t)((sample_fixed + (int64_t)FRAC_HALF) >> FRAC_BITS);
        } else {
          out[i + 1] = (int16_t)((sample_fixed - (int64_t)FRAC_HALF) >> FRAC_BITS);
        }
        pos_fixed += step;
      }
      // Sample 2
      {
        int left = (int)(pos_fixed >> FRAC_BITS);
        uint64_t frac = pos_fixed & (FRAC_ONE - 1);
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;
        int32_t s_left = src[left];
        int32_t s_right = src[right];
        int64_t sample_fixed = ((int64_t)s_left << FRAC_BITS) + (int64_t)(s_right - s_left) * (int64_t)frac;
        if (sample_fixed >= 0) {
          out[i + 2] = (int16_t)((sample_fixed + (int64_t)FRAC_HALF) >> FRAC_BITS);
        } else {
          out[i + 2] = (int16_t)((sample_fixed - (int64_t)FRAC_HALF) >> FRAC_BITS);
        }
        pos_fixed += step;
      }
      // Sample 3
      {
        int left = (int)(pos_fixed >> FRAC_BITS);
        uint64_t frac = pos_fixed & (FRAC_ONE - 1);
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;
        int32_t s_left = src[left];
        int32_t s_right = src[right];
        int64_t sample_fixed = ((int64_t)s_left << FRAC_BITS) + (int64_t)(s_right - s_left) * (int64_t)frac;
        if (sample_fixed >= 0) {
          out[i + 3] = (int16_t)((sample_fixed + (int64_t)FRAC_HALF) >> FRAC_BITS);
        } else {
          out[i + 3] = (int16_t)((sample_fixed - (int64_t)FRAC_HALF) >> FRAC_BITS);
        }
        pos_fixed += step;
      }
    }
    
    // Handle remaining samples
    for (; i < output_len; ++i) {
      int left = (int)(pos_fixed >> FRAC_BITS);
      uint64_t frac = pos_fixed & (FRAC_ONE - 1);
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      int32_t s_left = src[left];
      int32_t s_right = src[right];
      int64_t sample_fixed = ((int64_t)s_left << FRAC_BITS) + (int64_t)(s_right - s_left) * (int64_t)frac;
      if (sample_fixed >= 0) {
        out[i] = (int16_t)((sample_fixed + (int64_t)FRAC_HALF) >> FRAC_BITS);
      } else {
        out[i] = (int16_t)((sample_fixed - (int64_t)FRAC_HALF) >> FRAC_BITS);
      }
      pos_fixed += step;
    }
    
    hash = checksum_i16(out, (size_t)output_len);
  }
  
  free(out);
  return hash;
}