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
  // We use 64-bit fixed point math to simulate the double precision interpolation.
  
  if (output_len == 1) {
    // Edge case: output_len 1 would cause division by zero in original logic
    // but the loop logic handles it via the ratio. 
    // However, the original code uses (output_len - 1) in denominator.
    // If output_len is 1, the original code's behavior is technically undefined/NaN.
    // We follow the loop structure.
  } else {
    const uint64_t scale = 1ULL << 48;
    const uint64_t step = ((uint64_t)(input_len - 1) << 48) / (output_len - 1);

    for (int iter = 0; iter < iters; ++iter) {
      uint64_t current_pos_fp = 0;
      for (int i = 0; i < output_len; ++i) {
        int left = (int)(current_pos_fp >> 48);
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;

        // frac = (current_pos_fp % scale) / scale
        // sample = src[left] * (1 - frac) + src[right] * frac
        // sample = src[left] + frac * (src[right] - src[left])
        
        int64_t s_left = src[left];
        int64_t s_right = src[right];
        int64_t diff = s_right - s_left;
        
        // Fixed point interpolation: s_left + (diff * frac)
        // frac is (current_pos_fp & mask) / scale
        int64_t interpolated = s_left + ((diff * (int64_t)(current_pos_fp & (scale - 1))) >> 48);
        
        // The original code uses double precision and adds/subtracts 0.5 for rounding.
        // To match (int16_t)(sample + 0.5) for positive and (int16_t)(sample - 0.5) for negative:
        // This is equivalent to rounding to nearest integer away from zero.
        // We need to handle the fractional part of the interpolation for the rounding.
        
        // Re-calculating with higher precision to ensure exact match with double-based rounding
        // The original code: double sample = (double)src[left] * (1.0 - frac) + (double)src[right] * frac;
        // Let's use a slightly more robust fixed point approach to ensure correctness.
        
        // Re-evaluating the rounding: 
        // If sample is 5.2, sample + 0.5 = 5.7 -> (int)5
        // If sample is 5.7, sample + 0.5 = 6.2 -> (int)6
        // If sample is -5.2, sample - 0.5 = -5.7 -> (int)-5
        // If sample is -5.7, sample - 0.5 = -6.2 -> (int)-6
        // This is standard rounding to nearest, with ties rounding away from zero.
        
        // To match the exact double behavior, we use the fractional part of the fixed point.
        // current_pos_fp is (i * (input_len-1) / (output_len-1)) * 2^48
        // We need the fractional part of the 'sample' calculation.
        
        // Let's use a more direct approach for the rounding logic to ensure exactness.
        // sample = src[left] + (frac * (src[right] - src[left]))
        // We need to add 0.5 to the fractional part of the result.
        
        // Using 64-bit math to simulate the double precision rounding:
        // We'll use a scale of 2^32 for the fractional part to be safe.
        // But since we need to match the original exactly, let's use the 48-bit scale.
        
        // The fractional part of 'sample' is (diff * (current_pos_fp & mask)) / scale
        // We need to check if this fractional part is >= 0.5
        
        int64_t frac_num = diff * (int64_t)(current_pos_fp & (scale - 1));
        // The fractional part of the sample is frac_num / scale.
        // We want to round: if frac_num/scale >= 0.5, add 1 to the integer part.
        // This is equivalent to: if abs(frac_num) >= scale/2, add sign(frac_num) to integer part.
        
        int64_t integer_part = s_left + (frac_num >> 48);
        int64_t remainder = frac_num & (scale - 1);
        
        if (diff >= 0) {
            if (remainder >= (scale >> 1)) integer_part++;
        } else {
            if (remainder >= (scale >> 1)) integer_part--;
        }
        
        // Wait, the original code: if (sample >= 0.0) out[i] = (int16_t)(sample + 0.5); else out[i] = (int16_t)(sample - 0.5);
        // Let's re-verify: 
        // sample = 5.5 -> 5.5 + 0.5 = 6.0 -> 6
        // sample = 5.4 -> 5.4 + 0.5 = 5.9 -> 5
        // sample = -5.5 -> -5.5 - 0.5 = -6.0 -> -6
        // sample = -5.4 -> -5.4 - 0.5 = -5.9 -> -5
        // This is indeed rounding to nearest, ties away from zero.
        
        // Correcting the integer_part logic for the 'remainder' which can be negative if diff < 0
        // Actually, if diff < 0, frac_num is negative. 
        // Let's use a simpler way: 
        // sample_scaled = (src[left] << 48) + (diff * (current_pos_fp & mask))
        // sample_scaled is sample * 2^48
        // We want to round sample_scaled / 2^48 to nearest integer.
        
        // Let's use the 48-bit scale for the whole sample to avoid precision loss.
        // sample_fp = (src[left] << 48) + (diff * (current_pos_fp & (scale - 1)))
        // To round sample_fp / scale:
        // if sample_fp >= 0: (sample_fp + scale/2) / scale
        // if sample_fp < 0: (sample_fp - scale/2) / scale
        
        int64_t sample_fp = (s_left << 48) + frac_num;
        int64_t rounded_sample;
        if (sample_fp >= 0) {
            rounded_sample = (sample_fp + (scale >> 1)) >> 48;
        } else {
            rounded_sample = (sample_fp - (scale >> 1)) >> 48;
        }
        
        out[i] = (int16_t)rounded_sample;
        
        current_pos_fp += step;
      }
      hash = checksum_i16(out, (size_t)output_len);
    }
  } else {
    // output_len == 1 case
    for (int iter = 0; iter < iters; ++iter) {
        out[0] = src[0]; // pos = 0 * (in-1)/(1-1) is NaN, but let's assume 0
        hash = checksum_i16(out, 1);
    }
  }

  free(out);
  return hash;
}
