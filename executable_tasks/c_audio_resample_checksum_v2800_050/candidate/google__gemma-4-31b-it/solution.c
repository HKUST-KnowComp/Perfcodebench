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
  // We use a 32.32 fixed point representation for the position
  if (output_len == 1) {
    for (int iter = 0; iter < iters; ++iter) {
      out[0] = src[0];
      hash = checksum_i16(out, (size_t)output_len);
    }
  } else {
    uint64_t step_fixed = ((uint64_t)(input_len - 1) << 32) / (output_len - 1);
    
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t current_pos_fixed = 0;
      for (int i = 0; i < output_len; ++i) {
        uint32_t left = (uint32_t)(current_pos_fixed >> 32);
        uint32_t frac = (uint32_t)(current_pos_fixed & 0xFFFFFFFF);
        uint32_t right = left + 1;
        if (right >= (uint32_t)input_len) right = (uint32_t)input_len - 1;

        // Linear interpolation: src[left] + frac/2^32 * (src[right] - src[left])
        int32_t s_left = src[left];
        int32_t s_right = src[right];
        int64_t diff = (int64_t)s_right - s_left;
        int64_t interpolated = (int64_t)s_left + ((diff * frac) >> 32);

        // Rounding: (sample + 0.5) for positive, (sample - 0.5) for negative
        // In fixed point, 0.5 is 1 << 31 if we were using 32.32, but here 
        // the 'interpolated' value is already the integer part.
        // The fractional part of the interpolation is (diff * frac) & 0xFFFFFFFF
        uint64_t frac_part = (diff * frac) & 0xFFFFFFFF;
        
        if (interpolated >= 0) {
          out[i] = (int16_t)(interpolated + (frac_part >= 0x80000000));
        } else {
          out[i] = (int16_t)(interpolated - (frac_part >= 0x80000000));
        }
        
        current_pos_fixed += step_fixed;
      }
      hash = checksum_i16(out, (size_t)output_len);
    }
  }

  free(out);
  return hash;
}