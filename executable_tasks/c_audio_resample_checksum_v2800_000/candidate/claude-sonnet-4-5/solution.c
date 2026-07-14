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
  
  const int FRAC_BITS = 16;
  const int64_t FRAC_SCALE = 1LL << FRAC_BITS;
  const int64_t ROUND = FRAC_SCALE >> 1;
  
  int64_t pos_step = ((int64_t)(input_len - 1) << FRAC_BITS) / (output_len - 1);
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t pos_fixed = 0;
    for (int i = 0; i < output_len; ++i) {
      int left = (int)(pos_fixed >> FRAC_BITS);
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      
      int64_t frac = pos_fixed & (FRAC_SCALE - 1);
      int64_t inv_frac = FRAC_SCALE - frac;
      
      int64_t sample = ((int64_t)src[left] * inv_frac + (int64_t)src[right] * frac + ROUND) >> FRAC_BITS;
      
      out[i] = (int16_t)sample;
      pos_fixed += pos_step;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }
  
  free(out);
  return hash;
}
