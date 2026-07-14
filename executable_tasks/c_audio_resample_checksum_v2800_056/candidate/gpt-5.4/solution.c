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
  if (!src || input_len <= 0 || output_len <= 0) return 0;

  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  if (output_len == 1 || input_len == 1) {
    int16_t v = src[0];
    for (int i = 0; i < output_len; ++i) out[i] = v;
  } else {
    const int in_last = input_len - 1;
    const int out_last = output_len - 1;

    int left = 0;
    int64_t rem = 0;

    for (int i = 0; i < output_len; ++i) {
      int right = left + 1;
      if (right >= input_len) right = in_last;

      const int16_t s0 = src[left];
      const int16_t s1 = src[right];
      const int32_t diff = (int32_t)s1 - (int32_t)s0;

      int64_t num;
      if (diff >= 0) {
        num = (int64_t)s0 * (int64_t)out_last + (int64_t)diff * rem + (int64_t)out_last / 2;
      } else {
        num = (int64_t)s0 * (int64_t)out_last + (int64_t)diff * rem - (int64_t)out_last / 2;
      }
      out[i] = (int16_t)(num / (int64_t)out_last);

      rem += in_last;
      while (rem >= out_last) {
        rem -= out_last;
        ++left;
      }
      if (left > in_last) left = in_last;
    }
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}
