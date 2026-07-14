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
  uint64_t hash = 0;
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;
  
  const int32_t scale = ((int64_t)(input_len - 1) << 16) / (output_len - 1);
  
  for (int iter = 0; iter < iters; ++iter) {
    int i = 0;
    int main_len = output_len & ~3;
    
    for (; i < main_len; i += 4) {
      int32_t pos0 = ((int64_t)i * scale) >> 16;
      int32_t pos1 = ((int64_t)(i+1) * scale) >> 16;
      int32_t pos2 = ((int64_t)(i+2) * scale) >> 16;
      int32_t pos3 = ((int64_t)(i+3) * scale) >> 16;
      
      int32_t frac0 = (((int64_t)i * scale) & 0xFFFF);
      int32_t frac1 = (((int64_t)(i+1) * scale) & 0xFFFF);
      int32_t frac2 = (((int64_t)(i+2) * scale) & 0xFFFF);
      int32_t frac3 = (((int64_t)(i+3) * scale) & 0xFFFF);
      
      int32_t right0 = pos0 + (pos0 < input_len - 1);
      int32_t right1 = pos1 + (pos1 < input_len - 1);
      int32_t right2 = pos2 + (pos2 < input_len - 1);
      int32_t right3 = pos3 + (pos3 < input_len - 1);
      
      int32_t s0 = ((int32_t)src[pos0] * (65536 - frac0) + (int32_t)src[right0] * frac0 + 32768) >> 16;
      int32_t s1 = ((int32_t)src[pos1] * (65536 - frac1) + (int32_t)src[right1] * frac1 + 32768) >> 16;
      int32_t s2 = ((int32_t)src[pos2] * (65536 - frac2) + (int32_t)src[right2] * frac2 + 32768) >> 16;
      int32_t s3 = ((int32_t)src[pos3] * (65536 - frac3) + (int32_t)src[right3] * frac3 + 32768) >> 16;
      
      out[i] = (int16_t)s0;
      out[i+1] = (int16_t)s1;
      out[i+2] = (int16_t)s2;
      out[i+3] = (int16_t)s3;
    }
    
    for (; i < output_len; ++i) {
      int32_t pos_fixed = ((int64_t)i * scale);
      int32_t left = pos_fixed >> 16;
      int32_t right = left + (left < input_len - 1);
      int32_t frac = pos_fixed & 0xFFFF;
      int32_t sample = ((int32_t)src[left] * (65536 - frac) + (int32_t)src[right] * frac + 32768) >> 16;
      out[i] = (int16_t)sample;
    }
    
    hash = checksum_i16(out, (size_t)output_len);
  }
  
  free(out);
  return hash;
}