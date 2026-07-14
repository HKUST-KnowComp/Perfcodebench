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
  
  /* Use 32-bit fixed-point for fractional part (high precision) */
  const uint64_t scale = (uint64_t)1 << 32;
  const uint64_t step = ((uint64_t)(input_len - 1) * scale) / (uint64_t)(output_len - 1);
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t pos_fixed = 0;
    
    for (int i = 0; i < output_len; ++i) {
      int left = (int)(pos_fixed >> 32);
      uint32_t frac = (uint32_t)pos_fixed; /* lower 32 bits = fractional part */
      
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      
      int32_t s_left = src[left];
      int32_t s_right = src[right];
      
      /* Linear interpolation: left * (1 - frac) + right * frac */
      /* frac is in [0, 2^32), so frac/2^32 is the actual fraction */
      /* result = left + (right - left) * frac / 2^32 */
      int64_t diff = s_right - s_left;
      int64_t interp = ((int64_t)s_left << 32) + diff * (int64_t)frac;
      
      /* Round: add 0.5 (which is 1 << 31) for positive, subtract for negative */
      /* Then shift right by 32 */
      int64_t rounded;
      if (interp >= 0) {
        rounded = (interp + ((int64_t)1 << 31)) >> 32;
      } else {
        rounded = (interp - ((int64_t)1 << 31)) >> 32;
      }
      
      out[i] = (int16_t)rounded;
      pos_fixed += step;
    }
    
    hash = checksum_i16(out, (size_t)output_len);
  }
  
  free(out);
  return hash;
}