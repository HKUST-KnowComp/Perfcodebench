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
  
  if (output_len == 1) {
    for (int iter = 0; iter < iters; ++iter) {
      out[0] = src[0];
      hash = checksum_i16(out, 1);
    }
  } else {
    int64_t scale = ((int64_t)(input_len - 1) << 16) / (output_len - 1);
    
    for (int iter = 0; iter < iters; ++iter) {
      for (int i = 0; i < output_len; ++i) {
        int64_t pos_fixed = (int64_t)i * scale;
        int left = (int)(pos_fixed >> 16);
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;
        
        int frac = (int)(pos_fixed & 0xFFFF);
        int inv_frac = 0x10000 - frac;
        
        int32_t sample = ((int32_t)src[left] * inv_frac + (int32_t)src[right] * frac + 0x8000) >> 16;
        out[i] = (int16_t)sample;
      }
      hash = checksum_i16(out, (size_t)output_len);
    }
  }
  
  free(out);
  return hash;
}