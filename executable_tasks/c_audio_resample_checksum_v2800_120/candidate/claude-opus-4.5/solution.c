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
  if (output_len <= 0 || input_len <= 0) return 0;
  
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;
  
  uint64_t hash = 0;
  
  /* Fixed-point with 32 fractional bits */
  const uint64_t frac_bits = 32;
  const uint64_t one = (uint64_t)1 << frac_bits;
  const uint64_t half = (uint64_t)1 << (frac_bits - 1);
  
  /* step = (input_len - 1) / (output_len - 1) in fixed-point */
  uint64_t step = 0;
  if (output_len > 1) {
    step = ((uint64_t)(input_len - 1) << frac_bits) / (uint64_t)(output_len - 1);
  }
  
  const int max_left = input_len - 2;
  const int last_idx = input_len - 1;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t pos_fixed = 0;
    int i = 0;
    int out4 = output_len & ~3;
    
    for (; i < out4; i += 4) {
      /* Sample 0 */
      int left0 = (int)(pos_fixed >> frac_bits);
      uint64_t frac0 = pos_fixed & (one - 1);
      if (left0 > max_left) left0 = max_left;
      int right0 = left0 + 1;
      if (right0 > last_idx) right0 = last_idx;
      int32_t s_left0 = src[left0];
      int32_t s_right0 = src[right0];
      int64_t interp0 = ((int64_t)s_left0 << frac_bits) + (int64_t)(s_right0 - s_left0) * (int64_t)frac0;
      int64_t rounded0 = (interp0 >= 0) ? (interp0 + (int64_t)half) : (interp0 - (int64_t)half);
      out[i] = (int16_t)(rounded0 >> frac_bits);
      pos_fixed += step;
      
      /* Sample 1 */
      int left1 = (int)(pos_fixed >> frac_bits);
      uint64_t frac1 = pos_fixed & (one - 1);
      if (left1 > max_left) left1 = max_left;
      int right1 = left1 + 1;
      if (right1 > last_idx) right1 = last_idx;
      int32_t s_left1 = src[left1];
      int32_t s_right1 = src[right1];
      int64_t interp1 = ((int64_t)s_left1 << frac_bits) + (int64_t)(s_right1 - s_left1) * (int64_t)frac1;
      int64_t rounded1 = (interp1 >= 0) ? (interp1 + (int64_t)half) : (interp1 - (int64_t)half);
      out[i + 1] = (int16_t)(rounded1 >> frac_bits);
      pos_fixed += step;
      
      /* Sample 2 */
      int left2 = (int)(pos_fixed >> frac_bits);
      uint64_t frac2 = pos_fixed & (one - 1);
      if (left2 > max_left) left2 = max_left;
      int right2 = left2 + 1;
      if (right2 > last_idx) right2 = last_idx;
      int32_t s_left2 = src[left2];
      int32_t s_right2 = src[right2];
      int64_t interp2 = ((int64_t)s_left2 << frac_bits) + (int64_t)(s_right2 - s_left2) * (int64_t)frac2;
      int64_t rounded2 = (interp2 >= 0) ? (interp2 + (int64_t)half) : (interp2 - (int64_t)half);
      out[i + 2] = (int16_t)(rounded2 >> frac_bits);
      pos_fixed += step;
      
      /* Sample 3 */
      int left3 = (int)(pos_fixed >> frac_bits);
      uint64_t frac3 = pos_fixed & (one - 1);
      if (left3 > max_left) left3 = max_left;
      int right3 = left3 + 1;
      if (right3 > last_idx) right3 = last_idx;
      int32_t s_left3 = src[left3];
      int32_t s_right3 = src[right3];
      int64_t interp3 = ((int64_t)s_left3 << frac_bits) + (int64_t)(s_right3 - s_left3) * (int64_t)frac3;
      int64_t rounded3 = (interp3 >= 0) ? (interp3 + (int64_t)half) : (interp3 - (int64_t)half);
      out[i + 3] = (int16_t)(rounded3 >> frac_bits);
      pos_fixed += step;
    }
    
    for (; i < output_len; ++i) {
      int left = (int)(pos_fixed >> frac_bits);
      uint64_t frac = pos_fixed & (one - 1);
      if (left > max_left) left = max_left;
      int right = left + 1;
      if (right > last_idx) right = last_idx;
      int32_t s_left = src[left];
      int32_t s_right = src[right];
      int64_t interp = ((int64_t)s_left << frac_bits) + (int64_t)(s_right - s_left) * (int64_t)frac;
      int64_t rounded = (interp >= 0) ? (interp + (int64_t)half) : (interp - (int64_t)half);
      out[i] = (int16_t)(rounded >> frac_bits);
      pos_fixed += step;
    }
    
    hash = checksum_i16(out, (size_t)output_len);
  }
  
  free(out);
  return hash;
}