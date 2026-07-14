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
  if (!src || input_len <= 0 || output_len <= 0 || iters <= 0) return 0;

  size_t out_count = (size_t)output_len;
  int16_t* out = (int16_t*)malloc(out_count * sizeof(int16_t));
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

    for (int i = 0; i < output_len; ++i) {
      int right = left + 1;
      if (right >= input_len) right = in_last;

      int32_t a = (int32_t)src[left];
      int32_t b = (int32_t)src[right];
      int32_t diff = b - a;

      int64_t num = (int64_t)a * out_last + (int64_t)diff * rem;
      if (num >= 0) {
        out[i] = (int16_t)((num + (out_last / 2)) / out_last);
      } else {
        out[i] = (int16_t)((num - (out_last / 2)) / out_last);
      }

      rem += in_last;
      while (rem >= out_last) {
        rem -= out_last;
        ++left;
      }
    }
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_i16(out, out_count);
  }

  free(out);
  return hash;
}
