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
  if (src == NULL || input_len <= 0 || output_len <= 0 || iters <= 0) {
    return 0;
  }

  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  if (input_len == 1 || output_len == 1) {
    int16_t v = src[0];
    for (int i = 0; i < output_len; ++i) out[i] = v;
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = checksum_i16(out, (size_t)output_len);
    }
    free(out);
    return hash;
  }

  const int in_last = input_len - 1;
  const int out_last = output_len - 1;

  int* left_idx = (int*)malloc((size_t)output_len * sizeof(int));
  uint32_t* frac_q32 = (uint32_t*)malloc((size_t)output_len * sizeof(uint32_t));
  if (!left_idx || !frac_q32) {
    free(left_idx);
    free(frac_q32);
    free(out);
    return 0;
  }

  {
    uint64_t num = 0;
    const uint64_t step = (((uint64_t)in_last) << 32) / (uint64_t)out_last;
    for (int i = 0; i < output_len; ++i) {
      int left = (int)(num >> 32);
      uint32_t frac = (uint32_t)num;
      if (left >= in_last) {
        left = in_last;
        frac = 0;
      }
      left_idx[i] = left;
      frac_q32[i] = frac;
      num += step;
    }
    left_idx[out_last] = in_last;
    frac_q32[out_last] = 0;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      int left = left_idx[i];
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;

      int32_t a = (int32_t)src[left];
      int32_t b = (int32_t)src[right];
      int32_t diff = b - a;
      uint32_t frac = frac_q32[i];

      int64_t interp = ((int64_t)diff * (int64_t)frac);
      int32_t rounded;
      if (interp >= 0) {
        rounded = a + (int32_t)((interp + 0x80000000LL) >> 32);
      } else {
        rounded = a + (int32_t)((interp - 0x80000000LL) >> 32);
      }
      out[i] = (int16_t)rounded;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(frac_q32);
  free(left_idx);
  free(out);
  return hash;
}
