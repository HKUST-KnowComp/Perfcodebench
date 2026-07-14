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
  if (iters <= 0) return 0;
  if (src == NULL || input_len <= 0 || output_len <= 0) return 0;

  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  uint64_t hash = 0;

  if (output_len == 1 || input_len == 1) {
    const int16_t v = src[0];
    for (int i = 0; i < output_len; ++i) out[i] = v;
    hash = checksum_i16(out, (size_t)output_len);
    free(out);
    return hash;
  }

  const int in_last = input_len - 1;
  const int out_last = output_len - 1;

  for (int iter = 0; iter < iters; ++iter) {
    int left = 0;
    int64_t rem = 0;
    const int64_t step_num = (int64_t)in_last;
    const int64_t step_den = (int64_t)out_last;

    for (int i = 0; i < output_len; ++i) {
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;

      const int32_t a = (int32_t)src[left];
      const int32_t b = (int32_t)src[right];
      const int32_t diff = b - a;

      const int64_t num = (int64_t)a * step_den + (int64_t)diff * rem;
      if (num >= 0) {
        out[i] = (int16_t)((num + (step_den / 2)) / step_den);
      } else {
        out[i] = (int16_t)((num - (step_den / 2)) / step_den);
      }

      rem += step_num;
      if (rem >= step_den) {
        const int64_t adv = rem / step_den;
        left += (int)adv;
        rem -= adv * step_den;
        if (left > in_last) {
          left = in_last;
          rem = 0;
        }
      }
    }

    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}
