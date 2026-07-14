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
  // We use a large scale to maintain precision for the fractional part
  // The formula is: pos = i * (input_len - 1) / (output_len - 1)
  // We want: left = floor(pos), frac = pos - left
  // Let scale = 2^32
  // pos_scaled = i * (input_len - 1) * scale / (output_len - 1)
  
  if (output_len == 1) {
    for (int iter = 0; iter < iters; ++iter) {
      out[0] = src[0];
      hash = checksum_i16(out, (size_t)output_len);
    }
  } else {
    const uint64_t scale = 1ULL << 32;
    const uint64_t step_num = (uint64_t)(input_len - 1) * scale;
    const uint64_t step_den = (uint64_t)(output_len - 1);

    for (int iter = 0; iter < iters; ++iter) {
      for (int i = 0; i < output_len; ++i) {
        uint64_t pos_scaled = (uint64_t)i * step_num / step_den;
        int left = (int)(pos_scaled >> 32);
        uint32_t frac_scaled = (uint32_t)(pos_scaled & 0xFFFFFFFFULL);
        
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;

        // Linear interpolation: sample = src[left]*(1-frac) + src[right]*frac
        // sample = src[left] + frac * (src[right] - src[left])
        // Using 64-bit to prevent overflow during intermediate calculation
        int64_t s_left = src[left];
        int64_t s_right = src[right];
        int64_t diff = s_right - s_left;
        
        // frac is [0, 1), represented as [0, 2^32)
        // We need to add 0.5 for rounding. In fixed point, 0.5 is 2^31
        // result = s_left + (diff * frac_scaled) / 2^32
        // To round: result = (s_left * 2^32 + diff * frac_scaled + 2^31) / 2^32
        // However, the original code uses: if (sample >= 0) sample + 0.5 else sample - 0.5
        // This is equivalent to floor(sample + 0.5) for positive and ceil(sample - 0.5) for negative.
        // Actually, the original code is: (int16_t)(sample + 0.5) if sample >= 0 else (int16_t)(sample - 0.5)
        // This is standard rounding towards nearest integer, with ties away from zero.
        
        int64_t interpolated_scaled = (s_left << 32) + (diff * (int64_t)frac_scaled);
        // To implement the specific rounding logic: 
        // if sample >= 0: floor(sample + 0.5) -> (interpolated_scaled + 2^31) >> 32
        // if sample < 0: ceil(sample - 0.5) -> (interpolated_scaled - 2^31) >> 32 (Wait, let's re-check)
        // Original: if sample >= 0: (int16_t)(sample + 0.5). If sample = -0.5, result = -1.0? No, -0.5 - 0.5 = -1.0.
        // Let's use the exact logic: 
        // sample = interpolated_scaled / 2^32
        // if sample >= 0: out = (interpolated_scaled + 2^31) >> 32
        // else: out = (interpolated_scaled - 2^31) >> 32 (Wait, if sample = -0.1, sample - 0.5 = -0.6, cast to int is -0.6? No, cast to int is -0.6 -> 0? No, double to int is truncation.)
        // Let's re-read: (int16_t)(sample + 0.5) where sample is double.
        // If sample = -0.1, sample + 0.5 = 0.4, (int16_t)0.4 = 0.
        // If sample = -0.6, sample + 0.5 = -0.1, (int16_t)-0.1 = 0.
        // If sample = -0.7, sample + 0.5 = -0.2, (int16_t)-0.2 = 0.
        // Wait, the original code: if (sample >= 0.0) { out = sample + 0.5 } else { out = sample - 0.5 }
        // If sample = -0.1: sample < 0, so out = (int16_t)(-0.1 - 0.5) = (int16_t)(-0.6) = 0.
        // If sample = -0.6: sample < 0, so out = (int16_t)(-0.6 - 0.5) = (int16_t)(-1.1) = -1.
        // This is equivalent to: if sample >= 0: floor(sample + 0.5), else: ceil(sample - 0.5).
        // In fixed point (scale 2^32):
        // if interpolated_scaled >= 0: (interpolated_scaled + 2^31) >> 32
        // else: (interpolated_scaled - 2^31) >> 32 (Wait, -1.1 truncated is -1. Correct.)
        
        if (interpolated_scaled >= 0) {
            out[i] = (int16_t)((interpolated_scaled + (1LL << 31)) >> 32);
        } else {
            out[i] = (int16_t)((interpolated_scaled - (1LL << 31)) >> 32);
        }
      }
      hash = checksum_i16(out, (size_t)output_len);
    }
  }

  free(out);
  return hash;
}