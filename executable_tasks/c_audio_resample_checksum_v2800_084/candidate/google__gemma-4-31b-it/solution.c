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
  
  // Precompute fixed-point step
  // pos = i * (input_len - 1) / (output_len - 1)
  // We use a 64-bit fixed point representation for the position.
  // Let's use 32 bits for the fractional part.
  const int64_t scale = 1LL << 32;
  const int64_t step = (output_len > 1) ? 
      ((int64_t)(input_len - 1) * scale) / (output_len - 1) : 0;

  for (int iter = 0; iter < iters; ++iter) {
    if (output_len == 1) {
      out[0] = src[0];
    } else {
      int64_t current_pos_fp = 0;
      for (int i = 0; i < output_len; ++i) {
        int left = (int)(current_pos_fp >> 32);
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;
        
        int64_t frac_fp = current_pos_fp & 0xFFFFFFFF;
        
        // Linear interpolation: src[left] + frac * (src[right] - src[left])
        // sample = src[left] + (frac_fp * (src[right] - src[left])) / scale
        int64_t diff = src[right] - src[left];
        int64_t prod = diff * frac_fp;
        
        // Rounding: (prod + scale/2) / scale for positive, (prod - scale/2) / scale for negative
        int64_t rounded_offset;
        if (prod >= 0) {
          rounded_offset = (prod + (scale >> 1)) >> 32;
        } else {
          rounded_offset = (prod - (scale >> 1)) >> 32;
        }
        
        out[i] = (int16_t)(src[left] + rounded_offset);
        current_pos_fp += step;
      }
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}