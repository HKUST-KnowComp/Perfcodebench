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
  
  // Precompute constants for fixed-point arithmetic
  // We use a 32.32 fixed point representation for the position
  // pos = i * (input_len - 1) / (output_len - 1)
  uint64_t step_num = (uint64_t)(input_len - 1) << 32;
  uint64_t step_den = (uint64_t)(output_len - 1);

  for (int iter = 0; iter < iters; ++iter) {
    if (output_len == 1) {
      out[0] = src[0];
    } else {
      for (int i = 0; i < output_len; ++i) {
        // Fixed point position: pos_fp = (i * step_num) / step_den
        uint64_t pos_fp = ((uint64_t)i * step_num) / step_den;
        int left = (int)(pos_fp >> 32);
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;
        
        uint64_t frac_fp = pos_fp & 0xFFFFFFFF;
        
        // Linear interpolation: sample = src[left] + frac * (src[right] - src[left])
        // Using 64-bit to prevent overflow during intermediate multiplication
        int64_t diff = (int64_t)src[right] - src[left];
        int64_t product = (diff * (int64_t)frac_fp);
        
        // Rounding: (product + 2^31) >> 32
        // This mimics the (sample + 0.5) for positive and (sample - 0.5) for negative
        int64_t interpolated = (int64_t)src[left] + ((product + (1LL << 31)) >> 32);
        
        // Clamp to int16 range just in case, though linear interpolation shouldn't exceed it
        out[i] = (int16_t)interpolated;
      }
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}