#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  if (output_len <= 0 || input_len <= 0) return 0;
  
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;
  
  uint64_t hash = 0;
  
  // Precompute fixed-point step: (input_len - 1) / (output_len - 1)
  // Using 32 fractional bits for position
  const uint64_t scale_num = (uint64_t)(input_len - 1);
  const uint64_t scale_den = (uint64_t)(output_len - 1);
  
  for (int iter = 0; iter < iters; ++iter) {
    // Combined resampling and checksum in one pass
    hash = 1469598103934665603ULL;
    
    for (int i = 0; i < output_len; ++i) {
      // Calculate position using 64-bit arithmetic to avoid overflow
      // pos = i * (input_len - 1) / (output_len - 1)
      uint64_t num = (uint64_t)i * scale_num;
      int left = (int)(num / scale_den);
      uint64_t rem = num % scale_den;
      
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      
      // Linear interpolation: left * (1 - frac) + right * frac
      // frac = rem / scale_den
      // result = left_val + (right_val - left_val) * rem / scale_den
      int32_t left_val = src[left];
      int32_t right_val = src[right];
      int32_t diff = right_val - left_val;
      
      // Compute interpolated value with rounding
      // sample = left_val + diff * rem / scale_den
      // We need to round: add scale_den/2 before division for positive, subtract for negative
      int64_t interp = (int64_t)diff * (int64_t)rem;
      int32_t sample;
      
      if (interp >= 0) {
        sample = left_val + (int32_t)((interp + (int64_t)(scale_den / 2)) / (int64_t)scale_den);
      } else {
        sample = left_val + (int32_t)((interp - (int64_t)(scale_den / 2)) / (int64_t)scale_den);
      }
      
      // Clamp to int16_t range (shouldn't be needed for valid input, but safe)
      if (sample > 32767) sample = 32767;
      if (sample < -32768) sample = -32768;
      
      out[i] = (int16_t)sample;
      
      // Inline checksum update
      hash ^= (uint16_t)out[i];
      hash *= 1099511628211ULL;
    }
  }
  
  free(out);
  return hash;
}