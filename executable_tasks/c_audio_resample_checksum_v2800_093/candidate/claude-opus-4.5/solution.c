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

  for (int iter = 0; iter < iters; ++iter) {
    const int64_t scale_num = (int64_t)(input_len - 1);
    const int64_t scale_den = (int64_t)(output_len - 1);
    const int FRAC_BITS = 32;
    const int64_t ONE = (int64_t)1 << FRAC_BITS;
    const int64_t HALF = ONE >> 1;

    for (int i = 0; i < output_len; ++i) {
      int64_t pos_fixed = ((int64_t)i * scale_num * ONE) / scale_den;
      int left = (int)(pos_fixed >> FRAC_BITS);
      int64_t frac = pos_fixed & (ONE - 1);
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;

      int64_t left_val = (int64_t)src[left];
      int64_t right_val = (int64_t)src[right];
      int64_t sample_fixed = left_val * (ONE - frac) + right_val * frac;

      int16_t result;
      if (sample_fixed >= 0) {
        result = (int16_t)((sample_fixed + HALF) >> FRAC_BITS);
      } else {
        result = (int16_t)(-(((-sample_fixed) + HALF) >> FRAC_BITS));
      }
      out[i] = result;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}