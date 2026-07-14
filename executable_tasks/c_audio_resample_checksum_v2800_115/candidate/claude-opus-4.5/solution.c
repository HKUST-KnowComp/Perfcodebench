#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  size_t i = 0;
  size_t n4 = n & ~(size_t)3;
  for (; i < n4; i += 4) {
    hash ^= (uint16_t)data[i];
    hash *= 1099511628211ULL;
    hash ^= (uint16_t)data[i + 1];
    hash *= 1099511628211ULL;
    hash ^= (uint16_t)data[i + 2];
    hash *= 1099511628211ULL;
    hash ^= (uint16_t)data[i + 3];
    hash *= 1099511628211ULL;
  }
  for (; i < n; ++i) {
    hash ^= (uint16_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  uint64_t hash = 0;
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  const uint64_t FRAC_BITS = 32;
  const uint64_t FRAC_ONE = (uint64_t)1 << FRAC_BITS;
  const uint64_t FRAC_HALF = FRAC_ONE >> 1;

  uint64_t step = ((uint64_t)(input_len - 1) << FRAC_BITS) / (uint64_t)(output_len - 1);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t pos_fixed = 0;
    for (int i = 0; i < output_len; ++i) {
      int left = (int)(pos_fixed >> FRAC_BITS);
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      uint64_t frac = pos_fixed & (FRAC_ONE - 1);
      int32_t left_val = src[left];
      int32_t right_val = src[right];
      int64_t interp = ((int64_t)left_val << FRAC_BITS) + (int64_t)(right_val - left_val) * (int64_t)frac;
      int64_t rounded;
      if (interp >= 0) {
        rounded = (interp + (int64_t)FRAC_HALF) >> FRAC_BITS;
      } else {
        rounded = (interp - (int64_t)FRAC_HALF + 1) >> FRAC_BITS;
      }
      out[i] = (int16_t)rounded;
      pos_fixed += step;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}