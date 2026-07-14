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
  
  // Pre-calculate constants for fixed-point math
  // We use a large scale to maintain precision equivalent to double
  // The formula is: pos = i * (input_len - 1) / (output_len - 1)
  // Let scale = 1LL << 32
  // pos_fixed = (i * (input_len - 1) * scale) / (output_len - 1)
  
  if (output_len == 1) {
    for (int iter = 0; iter < iters; ++iter) {
      out[0] = src[0];
      hash = checksum_i16(out, (size_t)output_len);
    }
  } else {
    const uint64_t scale = 1ULL << 32;
    const uint64_t numerator_factor = (uint64_t)(input_len - 1) * scale;
    const uint64_t denominator = (uint64_t)(output_len - 1);

    for (int iter = 0; iter < iters; ++iter) {
      for (int i = 0; i < output_len; ++i) {
        uint64_t pos_fixed = ((uint64_t)i * numerator_factor) / denominator;
        int left = (int)(pos_fixed >> 32);
        uint32_t frac_fixed = (uint32_t)(pos_fixed & 0xFFFFFFFFULL);
        
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;

        // Linear interpolation: sample = src[left] + frac * (src[right] - src[left])
        // Using 64-bit to prevent overflow during intermediate steps
        int64_t s_left = src[left];
        int64_t s_right = src[right];
        
        // frac is [0, 1) in scale 2^32. 
        // To match the original (sample + 0.5) rounding logic:
        // We need to handle the rounding carefully. 
        // Original: sample = src[left]*(1-frac) + src[right]*frac
        // sample = src[left] + frac*(src[right] - src[left])
        
        int64_t diff = s_right - s_left;
        // frac_fixed is in [0, 2^32-1]. We want to add 0.5 in the scale.
        // The 0.5 offset in the original code is applied to the final double.
        // In fixed point, we add 1 << 31 to the numerator before dividing by 2^32.
        
        int64_t interpolated = s_left + ((diff * (int64_t)frac_fixed + (diff >= 0 ? 0x80000000LL : -0x80000000LL)) >> 32);
        
        // The original code uses: if (sample >= 0) sample + 0.5 else sample - 0.5
        // This is equivalent to rounding towards nearest, with halfway cases away from zero.
        // However, the fixed point math above with the offset handles the rounding.
        // Let's re-verify the rounding logic to match exactly.
        // Original: double sample = src[left] * (1.0 - frac) + src[right] * frac;
        // If sample = 10.5 -> 11. If sample = -10.5 -> -11.
        
        // Let's use a more direct approach for the rounding to ensure exact match:
        // sample_scaled = src[left] * (2^32 - frac_fixed) + src[right] * frac_fixed
        // sample = sample_scaled / 2^32
        // We want round(sample) with halfway away from zero.
        
        int64_t val_scaled = (s_left * (scale - frac_fixed)) + (s_right * frac_fixed);
        // To round val_scaled / scale to nearest integer with halfway away from zero:
        // if val_scaled >= 0: (val_scaled + scale/2) / scale
        // if val_scaled < 0:  (val_scaled - scale/2) / scale
        
        int64_t rounded;
        if (val_scaled >= 0) {
            rounded = (val_scaled + (1LL << 31)) >> 32;
        } else {
            rounded = (val_scaled - (1LL << 31)) >> 32;
        }
        out[i] = (int16_t)rounded;
      }
      hash = checksum_i16(out, (size_t)output_len);
    }
  }

  free(out);
  return hash;
}