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
  // We use a large scale to maintain precision equivalent to double
  // The original formula: pos = i * (input_len - 1) / (output_len - 1)
  // frac = pos - floor(pos)
  
  if (output_len == 1) {
    for (int iter = 0; iter < iters; ++iter) {
      out[0] = src[0];
      hash = checksum_i16(out, (size_t)output_len);
    }
  } else {
    const uint64_t scale = 1ULL << 32;
    const uint64_t step_num = (uint64_t)(input_len - 1) << 32;
    const uint64_t step_den = (uint64_t)(output_len - 1);

    for (int iter = 0; iter < iters; ++iter) {
      for (int i = 0; i < output_len; ++i) {
        // pos_scaled = i * (input_len - 1) / (output_len - 1) * 2^32
        uint64_t pos_scaled = (uint64_t)i * step_num / step_den;
        
        int left = (int)(pos_scaled >> 32);
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;
        
        // frac = (pos_scaled % 2^32) / 2^32
        uint64_t frac_scaled = pos_scaled & 0xFFFFFFFFULL;
        
        // sample = src[left] * (1 - frac) + src[right] * frac
        // To match the double precision rounding: 
        // sample = (src[left] * (2^32 - frac_scaled) + src[right] * frac_scaled) / 2^32
        // We use __int128 to prevent overflow during intermediate multiplication
        __int128 sample_fixed = (__int128)src[left] * (scale - frac_scaled) + (__int128)src[right] * frac_scaled;
        
        // The original code uses: if (sample >= 0.0) sample + 0.5 else sample - 0.5
        // This is equivalent to rounding to nearest integer (away from zero for .5)
        // In fixed point: (sample_fixed + (sign * 2^31)) >> 32
        int16_t val;
        if (sample_fixed >= 0) {
            val = (int16_t)((sample_fixed + (scale >> 1)) >> 32);
        } else {
            val = (int16_t)((sample_fixed - (scale >> 1)) >> 32);
        }
        out[i] = val;
      }
      hash = checksum_i16(out, (size_t)output_len);
    }
  }

  free(out);
  return hash;
}
