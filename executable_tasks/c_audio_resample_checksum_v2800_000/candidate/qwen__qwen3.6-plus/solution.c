#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint16_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  if (output_len <= 0) return 0;
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  int N = input_len - 1;
  int M = output_len - 1;

  int step_int = 0;
  int step_frac = 0;
  if (M > 0) {
    step_int = N / M;
    step_frac = N % M;
  }

  for (int iter = 0; iter < iters; ++iter) {
    int left = 0;
    int frac_num = 0;
    if (M == 0) {
      out[0] = src[0];
    } else {
      for (int i = 0; i < output_len; ++i) {
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;
        int diff = src[right] - src[left];
        int64_t num = (int64_t)diff * frac_num;
        int64_t rounded = (num >= 0) ? (num + M/2) / M : -((-num + M/2) / M);
        out[i] = (int16_t)(src[left] + rounded);

        frac_num += step_frac;
        left += step_int;
        if (frac_num >= M) {
          frac_num -= M;
          left += 1;
        }
      }
    }
  }

  uint64_t hash = checksum_i16(out, (size_t)output_len);
  free(out);
  return hash;
}