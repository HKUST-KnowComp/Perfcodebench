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
  // The original formula: pos = i * (input_len - 1) / (output_len - 1)
  // We use 64-bit fixed point math to simulate the double precision behavior.
  
  if (output_len == 1) {
    // Edge case: output_len 1 would cause division by zero in original logic
    // but the loop would handle it if we follow the logic carefully.
    // However, the original code uses (output_len - 1) in denominator.
    // If output_len is 1, the original code's behavior is technically undefined/NaN.
    // Given the context of resampling, we assume output_len > 1.
  } else {
    const uint64_t scale = 1ULL << 48;
    const uint64_t step = ((uint64_t)(input_len - 1) << 48) / (output_len - 1);

    for (int iter = 0; iter < iters; ++iter) {
      uint64_t current_pos_fp = 0;
      for (int i = 0; i < output_len; ++i) {
        // current_pos_fp represents 'pos' scaled by 2^48
        uint64_t left = current_pos_fp >> 48;
        uint64_t right = left + 1;
        if (right >= (uint64_t)input_len) right = input_len - 1;
        
        uint64_t frac_fp = current_pos_fp & ((1ULL << 48) - 1);
        
        // Interpolation: sample = src[left] * (1 - frac) + src[right] * frac
        // To match the original double precision rounding: 
        // sample = src[left] + frac * (src[right] - src[left])
        int64_t s_left = src[left];
        int64_t s_right = src[right];
        
        // We need to simulate: (double)sample + 0.5 (for positive) or - 0.5 (for negative)
        // This is equivalent to rounding to nearest integer.
        // Using fixed point: 
        // res = s_left + (frac_fp * (s_right - s_left)) / 2^48
        // To handle rounding: add 0.5 (which is 2^47 in our scale) before dividing
        
        int64_t diff = s_right - s_left;
        int64_t interpolated_scaled = (frac_fp * diff);
        
        // The original code uses: 
        // if (sample >= 0.0) out[i] = (int16_t)(sample + 0.5);
        // else out[i] = (int16_t)(sample - 0.5);
        // This is standard rounding.
        
        int64_t sample_val_scaled = (s_left << 48) + interpolated_scaled;
        // Rounding: add 2^47 if positive, subtract 2^47 if negative
        if (sample_val_scaled >= 0) {
            sample_val_scaled += (1ULL << 47);
        } else {
            sample_val_scaled -= (1ULL << 47);
        }
        
        out[i] = (int16_t)(sample_val_scaled >> 48);
        
        if (i < output_len - 1) {
            current_pos_fp += step;
        }
      }
      hash = checksum_i16(out, (size_t)output_len);
    }
  }

  free(out);
  return hash;
}