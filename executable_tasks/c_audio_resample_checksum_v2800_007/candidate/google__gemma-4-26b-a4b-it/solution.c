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
    // Based on original logic: pos = i * 0 / (output_len - 1) = 0
    // This results in out[i] = src[0]
    int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;
    for (int i = 0; i < output_len; ++i) out[i] = src[0];
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = checksum_i16(out, (size_t)output_len);
    }
    free(out);
    return hash;
  }

  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  // Pre-calculate fixed-point step
  // pos = i * (input_len - 1) / (output_len - 1)
  // We use 64-bit fixed point to maintain precision equivalent to double
  // Let scale = 2^32
  const uint64_t scale = 1ULL << 32;
  const uint64_t step = ((uint64_t)(input_len - 1) << 32) / (output_len - 1);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_pos_fp = 0;
    for (int i = 0; i < output_len; ++i) {
      // current_pos_fp represents pos * scale
      uint64_t left = current_pos_fp >> 32;
      uint64_t frac_fp = current_pos_fp & 0xFFFFFFFFULL;
      
      int left_idx = (int)left;
      int right_idx = left_idx + 1;
      if (right_idx >= input_len) right_idx = input_len - 1;

      // Interpolation: sample = src[left] * (1 - frac) + src[right] * frac
      // Using fixed point: sample = (src[left] * (scale - frac) + src[right] * frac) / scale
      // To match the original double precision rounding (sample + 0.5 or - 0.5):
      // We use 64-bit math for the intermediate product to avoid overflow.
      
      int64_t s_left = src[left_idx];
      int64_t s_right = src[right_idx];
      
      // frac is [0, 1), scale is 2^32
      // We need to perform: (s_left * (scale - frac_fp) + s_right * frac_fp) / scale
      // To handle the rounding (+0.5/-0.5), we add scale/2 before dividing.
      
      int64_t numerator = s_left * (scale - frac_fp) + s_right * frac_fp;
      int64_t sample_scaled = numerator + (s_left >= 0 ? (scale >> 1) : -(scale >> 1));
      // Note: The original code uses (sample + 0.5) for positive and (sample - 0.5) for negative.
      // This is equivalent to rounding towards nearest, with halfway cases rounding away from zero.
      // However, the original code's 'sample' is a double. 
      // Let's re-implement the exact logic: 
      // double sample = (double)src[left] * (1.0 - frac) + (double)src[right] * frac;
      // if (sample >= 0.0) out[i] = (int16_t)(sample + 0.5);
      // else out[i] = (int16_t)(sample - 0.5);
      
      // Correct fixed point approach for the specific rounding rule:
      // We need to calculate: floor(sample * scale + 0.5) for positive
      // and ceil(sample * scale - 0.5) for negative.
      // Actually, the original code is: (int16_t)(sample + 0.5) if sample >= 0
      // This is equivalent to: floor(sample + 0.5) for positive.
      // For negative: (int16_t)(sample - 0.5) is floor(sample - 0.5) if we consider truncation.
      // Wait, (int16_t) cast in C truncates towards zero.
      // If sample = -0.1, sample - 0.5 = -0.6, (int16_t)-0.6 = 0.
      // If sample = -0.6, sample - 0.5 = -1.1, (int16_t)-1.1 = -1.
      // Let's use the exact double logic but in fixed point to ensure correctness.
      
      // Re-evaluating the rounding: 
      // sample = (src[left] * (scale - frac_fp) + src[right] * frac_fp) / scale
      // If sample >= 0: out = (int16_t)(sample + 0.5) => floor(sample + 0.5)
      // If sample < 0: out = (int16_t)(sample - 0.5) => ceil(sample - 0.5)
      
      // Let's use the numerator directly to avoid division until the end.
      // numerator = sample * scale
      // If sample >= 0: out = (numerator + scale/2) / scale
      // If sample < 0: out = (numerator - scale/2) / scale (using truncation behavior)
      
      int64_t res;
      if (numerator >= 0) {
        res = (numerator + (scale >> 1)) / scale;
      } else {
        // For negative, (int16_t)(sample - 0.5) where sample is negative.
        // Example: sample = -0.1. sample - 0.5 = -0.6. (int16_t)-0.6 = 0.
        // Example: sample = -0.6. sample - 0.5 = -1.1. (int16_t)-1.1 = -1.
        // This is equivalent to: (numerator - scale/2) / scale with truncation.
        res = (numerator - (scale >> 1)) / scale;
      }
      out[i] = (int16_t)res;

      current_pos_fp += step;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}