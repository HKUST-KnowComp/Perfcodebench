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
  // We use a 32.32 fixed point representation for the position
  // pos = i * (input_len - 1) / (output_len - 1)
  uint64_t step_num = (uint64_t)(input_len - 1) << 32;
  uint64_t step_den = (uint64_t)(output_len - 1);
  
  // If output_len is 1, the loop for i < output_len handles it, 
  // but we must avoid division by zero.
  uint64_t step = (step_den == 0) ? 0 : (step_num / step_den);

  for (int iter = 0; iter < iters; ++iter) {
    if (output_len == 1) {
      out[0] = src[0];
    } else {
      for (int i = 0; i < output_len; ++i) {
        uint64_t pos_fixed = (uint64_t)i * step;
        int left = (int)(pos_fixed >> 32);
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;
        
        uint64_t frac_fixed = pos_fixed & 0xFFFFFFFFULL;
        
        // Linear interpolation: src[left] + frac * (src[right] - src[left])
        // Using 64-bit to prevent overflow: (src[right]-src[left]) * frac_fixed
        int64_t diff = (int64_t)src[right] - src[left];
        int64_t interp_fixed = ((diff * (int64_t)frac_fixed) >> 32);
        int64_t sample_fixed = (int64_t)src[left] + interp_fixed;
        
        // The original code uses double and adds/subtracts 0.5 for rounding.
        // In fixed point, the fractional part is in the lower bits of the multiplication.
        // To match the double precision rounding exactly:
        // sample = src[left] + (frac * diff)
        // We need the actual fractional part of (diff * frac_fixed) / 2^32
        int64_t rem = (diff * (int64_t)frac_fixed) & 0xFFFFFFFFULL;
        // Handle negative remainder for correct rounding
        if (diff < 0 && rem != 0) rem -= 0x100000000LL;

        // The original logic: if (sample >= 0) out = (int)(sample + 0.5) else (int)(sample - 0.5)
        // This is equivalent to rounding away from zero.
        // The fractional part is rem / 2^32. 
        // Rounding away from zero means if rem != 0, we move 1 unit in the direction of the sign.
        int64_t final_val = sample_fixed;
        if (rem > 0) final_val += 1;
        else if (rem < 0) final_val -= 1;
        
        out[i] = (int16_t)final_val;
      }
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}
