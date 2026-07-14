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
  uint64_t hash = 0;
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    if (output_len == 1) {
      out[0] = src[0];
    } else {
      const int64_t scale_num = (int64_t)(input_len - 1);
      const int64_t scale_den = (int64_t)(output_len - 1);
      const int FRAC_BITS = 32;
      const int64_t one = (int64_t)1 << FRAC_BITS;
      const int64_t half = (int64_t)1 << (FRAC_BITS - 1);

      for (int i = 0; i < output_len; ++i) {
        int64_t pos_num = (int64_t)i * scale_num;
        int left = (int)(pos_num / scale_den);
        int64_t left_pos = (int64_t)left * scale_den;
        int64_t frac_num = pos_num - left_pos;
        int64_t frac = (frac_num << FRAC_BITS) / scale_den;

        int right = left + 1;
        if (right >= input_len) right = input_len - 1;

        int64_t left_val = (int64_t)src[left];
        int64_t right_val = (int64_t)src[right];
        int64_t sample_fixed = left_val * (one - frac) + right_val * frac + half;
        int64_t sample = sample_fixed >> FRAC_BITS;
        out[i] = (int16_t)sample;
      }
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}
