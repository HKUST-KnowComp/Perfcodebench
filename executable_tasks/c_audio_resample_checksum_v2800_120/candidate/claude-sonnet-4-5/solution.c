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
    int i = 0;
    int limit = output_len - 3;
    
    // Unrolled loop processing 4 samples at a time
    for (; i < limit; i += 4) {
      // Sample 0
      uint64_t pos0 = (uint64_t)i * scale;
      int left0 = (int)(pos0 >> 16);
      int right0 = left0 + 1;
      if (right0 >= input_len) right0 = input_len - 1;
      uint32_t frac0 = (uint32_t)(pos0 & 0xFFFF);
      int32_t sample0 = ((int32_t)src[left0] * (int32_t)(65536 - frac0) + 
                         (int32_t)src[right0] * (int32_t)frac0 + 32768) >> 16;
      out[i] = (int16_t)sample0;
      
      // Sample 1
      uint64_t pos1 = (uint64_t)(i + 1) * scale;
      int left1 = (int)(pos1 >> 16);
      int right1 = left1 + 1;
      if (right1 >= input_len) right1 = input_len - 1;
      uint32_t frac1 = (uint32_t)(pos1 & 0xFFFF);
      int32_t sample1 = ((int32_t)src[left1] * (int32_t)(65536 - frac1) + 
                         (int32_t)src[right1] * (int32_t)frac1 + 32768) >> 16;
      out[i + 1] = (int16_t)sample1;
      
      // Sample 2
      uint64_t pos2 = (uint64_t)(i + 2) * scale;
      int left2 = (int)(pos2 >> 16);
      int right2 = left2 + 1;
      if (right2 >= input_len) right2 = input_len - 1;
      uint32_t frac2 = (uint32_t)(pos2 & 0xFFFF);
      int32_t sample2 = ((int32_t)src[left2] * (int32_t)(65536 - frac2) + 
                         (int32_t)src[right2] * (int32_t)frac2 + 32768) >> 16;
      out[i + 2] = (int16_t)sample2;
      
      // Sample 3
      uint64_t pos3 = (uint64_t)(i + 3) * scale;
      int left3 = (int)(pos3 >> 16);
      int right3 = left3 + 1;
      if (right3 >= input_len) right3 = input_len - 1;
      uint32_t frac3 = (uint32_t)(pos3 & 0xFFFF);
      int32_t sample3 = ((int32_t)src[left3] * (int32_t)(65536 - frac3) + 
                         (int32_t)src[right3] * (int32_t)frac3 + 32768) >> 16;
      out[i + 3] = (int16_t)sample3;
    }
    
    // Handle remaining samples
    for (; i < output_len; ++i) {
      uint64_t pos = (uint64_t)i * scale;
      int left = (int)(pos >> 16);
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      uint32_t frac = (uint32_t)(pos & 0xFFFF);
      int32_t sample = ((int32_t)src[left] * (int32_t)(65536 - frac) + 
                        (int32_t)src[right] * (int32_t)frac + 32768) >> 16;
      out[i] = (int16_t)sample;
    }
    
    hash = checksum_i16(out, (size_t)output_len);
  }
  
  free(out);
  return hash;
}