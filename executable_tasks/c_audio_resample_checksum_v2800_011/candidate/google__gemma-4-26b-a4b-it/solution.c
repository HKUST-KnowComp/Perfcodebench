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
  
  // Pre-calculate fixed-point constants to avoid double math in the loop
  // We use a large scale to maintain precision equivalent to the original double math
  // The original logic: pos = i * (input_len - 1) / (output_len - 1)
  // frac = pos - floor(pos)
  
  if (output_len == 1) {
    // Edge case: output_len 1 would cause division by zero in original logic
    // but based on the formula, it's usually handled by the harness.
    // We follow the loop logic: pos = 0 * ... = 0
    for (int iter = 0; iter < iters; ++iter) {
        out[0] = src[0];
        hash = checksum_i16(out, (size_t)output_len);
    }
  } else {
    const uint64_t scale = 1ULL << 32;
    const uint64_t step = ((uint64_t)(input_len - 1) << 32) / (output_len - 1);

    for (int iter = 0; iter < iters; ++iter) {
      uint64_t current_pos_scaled = 0;
      for (int i = 0; i < output_len; ++i) {
        int left = (int)(current_pos_scaled >> 32);
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;

        uint64_t frac_scaled = current_pos_scaled & 0xFFFFFFFFULL;
        
        // Linear interpolation: src[left] * (1 - frac) + src[right] * frac
        // Using 64-bit to prevent overflow during intermediate steps
        int64_t s_left = src[left];
        int64_t s_right = src[right];
        
        // frac is [0, 1) in scale 2^32. 
        // sample = s_left + (s_right - s_left) * frac
        int64_t sample_scaled = (s_left << 32) + ((s_right - s_left) * (int64_t)frac_scaled);
        
        // The original code uses: if (sample >= 0.0) sample + 0.5 else sample - 0.5
        // This is equivalent to rounding towards nearest integer with halfway cases away from zero.
        // In fixed point (scale 2^32), 0.5 is 1 << 31.
        int64_t rounded_sample;
        if (sample_scaled >= 0) {
            rounded_sample = (sample_scaled + (1ULL << 31)) >> 32;
        } else {
            rounded_sample = (sample_scaled - (1ULL << 31)) >> 32;
        }
        
        out[i] = (int16_t)rounded_sample;
        current_pos_scaled += step;
      }
      hash = checksum_i16(out, (size_t)output_len);
    }
  }

  free(out);
  return hash;
}