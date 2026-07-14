#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  size_t i = 0;
  size_t n4 = n & ~(size_t)3;
  for (; i < n4; i += 4) {
    hash ^= (uint16_t)data[i];
    hash *= prime;
    hash ^= (uint16_t)data[i + 1];
    hash *= prime;
    hash ^= (uint16_t)data[i + 2];
    hash *= prime;
    hash ^= (uint16_t)data[i + 3];
    hash *= prime;
  }
  for (; i < n; ++i) {
    hash ^= (uint16_t)data[i];
    hash *= prime;
  }
  return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  if (output_len <= 0) return 0;
  
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    if (output_len == 1) {
      out[0] = src[0];
    } else {
      const int64_t in_max = (int64_t)(input_len - 1);
      const int64_t out_max = (int64_t)(output_len - 1);
      
      for (int i = 0; i < output_len; ++i) {
        int64_t pos_num = (int64_t)i * in_max;
        int64_t left_idx = pos_num / out_max;
        int64_t frac_num = pos_num - left_idx * out_max;
        
        int left = (int)left_idx;
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;
        
        int32_t s_left = src[left];
        int32_t s_right = src[right];
        
        int64_t sample_num = (int64_t)s_left * out_max + (int64_t)(s_right - s_left) * frac_num;
        int64_t denom = out_max;
        
        int16_t result;
        if (sample_num >= 0) {
          result = (int16_t)((sample_num + denom / 2) / denom);
        } else {
          result = (int16_t)((sample_num - denom / 2) / denom);
        }
        out[i] = result;
      }
    }
    hash = checksum_i16(out, (size_t)output_len);
  }
  
  free(out);
  return hash;
}