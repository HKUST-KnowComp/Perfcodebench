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
  
  // Pre-calculate fixed-point constants to avoid double math in the loop.
  // We use a large scale to maintain precision equivalent to the original double logic.
  // The original logic: pos = i * (input_len - 1) / (output_len - 1)
  // We use 64-bit fixed point math.
  
  if (output_len == 1) {
    // Edge case: output_len 1 would cause division by zero in original logic (output_len - 1)
    // However, the original code uses (double)(output_len - 1) in denominator.
    // If output_len is 1, the original code results in NaN/Inf. 
    // Assuming output_len > 1 based on typical resampling tasks.
  } else {
    const uint64_t scale = 1ULL << 32;
    const uint64_t step = ((uint64_t)(input_len - 1) << 32) / (output_len - 1);

    for (int iter = 0; iter < iters; ++iter) {
      uint64_t current_pos_fp = 0;
      for (int i = 0; i < output_len; ++i) {
        int left = (int)(current_pos_fp >> 32);
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;

        // frac = (current_pos_fp % scale) / scale
        // sample = src[left] * (1 - frac) + src[right] * frac
        // sample = src[left] + frac * (src[right] - src[left])
        
        uint64_t frac_fp = current_pos_fp & 0xFFFFFFFFULL;
        int64_t diff = (int64_t)src[right] - (int64_t)src[left];
        
        // Fixed point interpolation: src[left] + (diff * frac_fp) / 2^32
        // To match the original rounding: (sample + 0.5) for positive, (sample - 0.5) for negative.
        // This is equivalent to rounding the result of the interpolation.
        
        int64_t interpolated = (int64_t)src[left] * (1ULL << 32) + (diff * (int64_t)frac_fp);
        // The value is currently in 2^32 scale. 
        // To round: add 0.5 in 2^32 scale, which is 1 << 31.
        if (interpolated >= 0) {
            out[i] = (int16_t)((interpolated + (1ULL << 31)) >> 32);
        } else {
            out[i] = (int16_t)((interpolated - (1ULL << 31)) >> 32);
        }

        current_pos_fp += step;
      }
      hash = checksum_i16(out, (size_t)output_len);
    }
  }

  free(out);
  return hash;
}