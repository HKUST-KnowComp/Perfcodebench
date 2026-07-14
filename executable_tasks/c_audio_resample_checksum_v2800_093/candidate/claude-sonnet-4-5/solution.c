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
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;
  
  uint64_t hash = 0;
  
  // Precompute scale factor in fixed-point (16.16 format)
  uint64_t scale = ((uint64_t)(input_len - 1) << 16) / (uint64_t)(output_len - 1);
  
  for (int iter = 0; iter < iters; ++iter) {
    int16_t* dst = out;
    uint64_t pos_fixed = 0;
    
    for (int i = 0; i < output_len; ++i) {
      int left = (int)(pos_fixed >> 16);
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      
      // Extract fractional part (16 bits)
      uint32_t frac = (uint32_t)(pos_fixed & 0xFFFF);
      
      // Linear interpolation in fixed-point
      int32_t s_left = src[left];
      int32_t s_right = src[right];
      int32_t diff = s_right - s_left;
      
      // Compute: s_left + diff * frac / 65536
      // Add 32768 for rounding (0.5 in 16-bit fixed-point)
      int32_t sample = s_left * 65536 + diff * (int32_t)frac + 32768;
      *dst++ = (int16_t)(sample >> 16);
      
      pos_fixed += scale;
    }
    
    hash = checksum_i16(out, (size_t)output_len);
  }
  
  free(out);
  return hash;
}