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
        uint32_t frac_fixed = (uint32_t)(current_pos_fixed & 0xFFFFFFFF);
        
        uint32_t right = left + 1;
        if (right >= (uint32_t)input_len) right = (uint32_t)input_len - 1;
        
        // Linear interpolation: src[left] + frac * (src[right] - src[left])
        // Using 64-bit to prevent overflow during multiplication
        int64_t diff = (int64_t)src[right] - src[left];
        int64_t sample_fixed = ((int64_t)src[left] << 32) + (diff * frac_fixed);
        
        // Rounding: (sample_fixed + 0.5 * 2^32) / 2^32
        // For positive: (val + 2^31) >> 32
        // For negative: (val - 2^31) >> 32
        if (sample_fixed >= 0) {
          out[i] = (int16_t)((sample_fixed + (1ULL << 31)) >> 32);
        } else {
          out[i] = (int16_t)((sample_fixed - (1ULL << 31)) >> 32);
        }
        
        current_pos_fixed += step_fixed;
      }
      hash = checksum_i16(out, (size_t)output_len);
    }
  }

  free(out);
  return hash;
}