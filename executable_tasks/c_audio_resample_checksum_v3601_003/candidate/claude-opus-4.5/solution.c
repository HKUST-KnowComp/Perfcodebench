#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  uint64_t hash = 0;
  
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;
  
  /* Fixed-point arithmetic with 32-bit fractional precision */
  const uint64_t scale = ((uint64_t)(input_len - 1) << 32) / (uint64_t)(output_len - 1);
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t pos_fixed = 0;
    
    for (int i = 0; i < output_len; ++i) {
      int left = (int)(pos_fixed >> 32);
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      
      /* Extract 15-bit fraction for interpolation (sufficient precision) */
      uint32_t frac = (uint32_t)((pos_fixed >> 17) & 0x7FFF);
      
      int32_t s_left = src[left];
      int32_t s_right = src[right];
      
      /* Linear interpolation: left * (1 - frac) + right * frac */
      /* = left + frac * (right - left) */
      /* frac is in [0, 32767] representing [0, 1) */
      int32_t diff = s_right - s_left;
      int32_t interp = s_left * 32768 + diff * (int32_t)frac;
      
      /* Round to nearest: add half, then shift */
      int32_t sample;
      if (interp >= 0) {
        sample = (interp + 16384) >> 15;
      } else {
        sample = -(((-interp) + 16384) >> 15);
      }
      
      out[i] = (int16_t)sample;
      pos_fixed += scale;
    }
    
    /* Inline checksum */
    hash = 1469598103934665603ULL;
    for (int i = 0; i < output_len; ++i) {
      hash ^= (uint16_t)out[i];
      hash *= 1099511628211ULL;
    }
  }
  
  free(out);
  return hash;
}