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
  if (input_len <= 1) {
    // Handle edge case where input_len - 1 would be zero
    // The original code would result in NaN/Inf via double division.
    // However, based on the original logic, we follow the loop structure.
  }

  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  uint64_t hash = 0;
  
  // Pre-calculate fixed-point constants to avoid double math in the loop
  // We use a large scale for fixed-point precision (2^32)
  const uint64_t SCALE = 1ULL << 32;
  
  // The original formula: pos = i * (input_len - 1) / (output_len - 1)
  // We want to avoid division in the loop. 
  // Let's use a 64-bit fixed point representation for the step.
  
  for (int iter = 0; iter < iters; ++iter) {
    if (output_len == 1) {
        out[0] = src[0];
    } else {
        // step = (input_len - 1) / (output_len - 1) in fixed point
        // We use 64-bit to prevent overflow during intermediate steps
        uint64_t step_fp = ((uint64_t)(input_len - 1) << 32) / (uint64_t)(output_len - 1);
        uint64_t current_pos_fp = 0;

        for (int i = 0; i < output_len; ++i) {
            int left = (int)(current_pos_fp >> 32);
            int right = left + 1;
            if (right >= input_len) right = input_len - 1;

            // frac = (current_pos_fp % 2^32) / 2^32
            uint64_t frac_fp = current_pos_fp & 0xFFFFFFFFULL;
            
            // sample = src[left] * (1 - frac) + src[right] * frac
            // To preserve the exact rounding behavior of the original:
            // original: sample = src[left] + frac * (src[right] - src[left])
            // original rounding: (int16_t)(sample + 0.5) if sample >= 0 else (int16_t)(sample - 0.5)
            
            int64_t s_left = src[left];
            int64_t s_right = src[right];
            int64_t diff = s_right - s_left;
            
            // Fixed point interpolation: s_left + (diff * frac_fp) / 2^32
            // To handle the +0.5 / -0.5 rounding correctly:
            // We need to add 0.5 to the result. In fixed point, 0.5 is 1 << 31.
            // However, the original code does the rounding on the floating point result.
            // Let's re-implement the rounding logic precisely.
            
            // The original code: 
            // double sample = (double)src[left] * (1.0 - frac) + (double)src[right] * frac;
            // if (sample >= 0.0) out[i] = (int16_t)(sample + 0.5);
            // else out[i] = (int16_t)(sample - 0.5);
            
            // This is equivalent to: floor(sample + 0.5) for positive, ceil(sample - 0.5) for negative.
            // Actually, it's simpler: it's rounding to nearest integer, with ties away from zero.
            
            // Using 64-bit math to simulate the double precision rounding:
            // sample_scaled = src[left] * 2^32 + diff * frac_fp
            // sample = sample_scaled / 2^32
            // rounding: (sample_scaled + (sign * 2^31)) / 2^32
            
            int64_t sample_scaled = (s_left << 32) + (diff * (int64_t)frac_fp);
            int64_t rounded_sample;
            if (sample_scaled >= 0) {
                rounded_sample = (sample_scaled + (1LL << 31)) >> 32;
            } else {
                rounded_sample = (sample_scaled - (1LL << 31)) >> 32;
            }
            out[i] = (int16_t)rounded_sample;

            current_pos_fp += step_fp;
        }
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}
