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
    // The original code would result in NaN/Inf due to division by zero if output_len > 1
    // However, following the original logic's structure:
    int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;
    for (int i = 0; i < output_len; ++i) out[i] = src[0];
    uint64_t hash = checksum_i16(out, (size_t)output_len);
    free(out);
    return hash;
  }

  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  // Pre-calculate fixed-point constants
  // We use 64-bit fixed point to maintain precision equivalent to double
  // The original formula: pos = i * (input_len - 1) / (output_len - 1)
  // We want to represent 'pos' and 'frac' accurately.
  // Let's use a large scale factor for fixed point.
  const uint64_t SCALE = 1ULL << 32;
  const uint64_t step_scaled = ((uint64_t)(input_len - 1) << 32) / (output_len - 1);

  uint64_t total_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_pos_scaled = 0;
    for (int i = 0; i < output_len; ++i) {
      // pos = i * (input_len - 1) / (output_len - 1)
      // We calculate current_pos_scaled = pos * 2^32
      // To avoid drift, we calculate it directly from i
      uint64_t pos_scaled = ((uint64_t)i * (uint64_t)(input_len - 1) << 32) / (output_len - 1);
      
      int left = (int)(pos_scaled >> 32);
      uint32_t frac_scaled = (uint32_t)(pos_scaled & 0xFFFFFFFFULL);
      
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;

      // Original: sample = src[left] * (1.0 - frac) + src[right] * frac
      // Using fixed point: 
      // sample = (src[left] * (SCALE - frac_scaled) + src[right] * frac_scaled) / SCALE
      
      int64_t s_left = src[left];
      int64_t s_right = src[right];
      
      // We need to handle the rounding (+0.5 or -0.5) correctly.
      // The original code: if (sample >= 0) out = sample + 0.5 else out = sample - 0.5
      // This is equivalent to floor(sample + 0.5) for positive and ceil(sample - 0.5) for negative.
      // In integer math: (val + (1 << 31)) >> 32 is not quite right for signed.
      // Let's use the property: round(x) = floor(x + 0.5)
      // For our fixed point: (numerator + (1 << 31)) >> 32
      
      int64_t numerator = s_left * (SCALE - (int64_t)frac_scaled) + s_right * (int64_t)frac_scaled;
      
      // To match the exact behavior of (double)sample + 0.5 (for positive) and - 0.5 (for negative):
      // The original code uses: if (sample >= 0.0) out = (int16_t)(sample + 0.5) else out = (int16_t)(sample - 0.5)
      // This is actually a symmetric rounding towards infinity.
      
      int64_t rounded_sample;
      if (numerator >= 0) {
        rounded_sample = (numerator + (1LL << 31)) >> 32;
      } else {
        rounded_sample = (numerator - (1LL << 31)) >> 32;
      }
      
      out[i] = (int16_t)rounded_sample;
    }
    total_hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return total_hash;
}
