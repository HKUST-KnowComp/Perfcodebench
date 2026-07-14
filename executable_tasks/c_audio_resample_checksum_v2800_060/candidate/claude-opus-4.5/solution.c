#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  if (output_len <= 0 || input_len <= 0) return 0;
  
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;
  
  uint64_t hash = 0;
  
  /* Use fixed-point arithmetic with 32-bit fractional precision */
  const uint64_t scale = (uint64_t)(input_len - 1) << 32;
  const uint64_t step = scale / (uint64_t)(output_len - 1);
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t pos_fixed = 0;
    hash = 1469598103934665603ULL;
    
    for (int i = 0; i < output_len; ++i) {
      int left = (int)(pos_fixed >> 32);
      uint32_t frac = (uint32_t)pos_fixed; /* fractional part in 0..2^32-1 */
      
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      
      int32_t s_left = src[left];
      int32_t s_right = src[right];
      
      /* Linear interpolation: sample = s_left + (s_right - s_left) * frac */
      /* frac is in [0, 2^32), so we compute (diff * frac) >> 32 */
      int64_t diff = (int64_t)(s_right - s_left);
      int64_t interp = (diff * (int64_t)frac) >> 32;
      int32_t sample = s_left + (int32_t)interp;
      
      /* Rounding: the fixed-point interpolation already gives us the floor,
         we need to add 0.5 and truncate for proper rounding.
         Since frac represents the fractional position, we check if the
         remainder would round up. */
      /* For exact match with original: compute with higher precision */
      /* Actually, let's compute the rounding more carefully */
      
      /* The fractional part that would cause rounding:
         original: sample + 0.5 for positive, sample - 0.5 for negative
         This is equivalent to: (sample * 2 + sign) / 2 with integer division */
      
      /* Recompute with proper rounding to match original */
      /* sample_scaled = s_left * (1-frac) + s_right * frac
         = s_left + (s_right - s_left) * frac
         where frac = (pos_fixed & 0xFFFFFFFF) / 2^32 */
      
      /* For rounding: we need the fractional part of the result */
      /* Let's use 48-bit precision for the interpolation */
      int64_t sample_48 = ((int64_t)s_left << 16) + ((diff * (int64_t)frac) >> 16);
      /* Now sample_48 has 16 fractional bits */
      /* Round to nearest: add 0.5 (which is 1 << 15) and shift */
      int16_t rounded;
      if (sample_48 >= 0) {
        rounded = (int16_t)((sample_48 + (1 << 15)) >> 16);
      } else {
        rounded = (int16_t)((sample_48 - (1 << 15)) >> 16);
      }
      
      out[i] = rounded;
      
      /* Inline checksum computation */
      hash ^= (uint16_t)rounded;
      hash *= 1099511628211ULL;
      
      pos_fixed += step;
    }
  }
  
  free(out);
  return hash;
}