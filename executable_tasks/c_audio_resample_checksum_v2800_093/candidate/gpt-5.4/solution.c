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
  if (src == NULL || input_len <= 0 || output_len <= 0 || iters <= 0) return 0;

  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  if (output_len == 1) {
    out[0] = src[0];
  } else if (input_len == 1) {
    int16_t v = src[0];
    for (int i = 0; i < output_len; ++i) out[i] = v;
  } else {
    const int in_last = input_len - 1;
    const int out_last = output_len - 1;

    int left = 0;
    int64_t rem = 0;
    const int64_t num = (int64_t)in_last;
    const int64_t den = (int64_t)out_last;

    for (int i = 0; i < output_len; ++i) {
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;

      int16_t a = src[left];
      int16_t b = src[right];
      int32_t diff = (int32_t)b - (int32_t)a;

      int64_t numer = (int64_t)a * den + (int64_t)diff * rem;
      if (numer >= 0) {
        out[i] = (int16_t)((numer + den / 2) / den);
      } else {
        out[i] = (int16_t)((numer - den / 2) / den);
      }

      rem += num;
      if (rem >= den) {
        int64_t step = rem / den;
        left += (int)step;
        rem -= step * den;
      }
    }
  }

  uint64_t hash = checksum_i16(out, (size_t)output_len);
  for (int iter = 1; iter < iters; ++iter) {
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}
