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

  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  if (output_len == 1 || input_len == 1) {
    int16_t v = src[0];
    for (int i = 0; i < output_len; ++i) out[i] = v;
  } else {
    const int in_last = input_len - 1;
    const int out_last = output_len - 1;

    for (int i = 0; i < output_len; ++i) {
      const long long num = (long long)i * (long long)in_last;
      const int left = (int)(num / out_last);
      const int rem = (int)(num - (long long)left * out_last);
      const int right = (left < in_last) ? (left + 1) : left;

      const int32_t a = (int32_t)src[left];
      const int32_t b = (int32_t)src[right];
      const int32_t diff = b - a;

      int32_t val_num = a * out_last + diff * rem;
      if (val_num >= 0) {
        out[i] = (int16_t)((val_num + out_last / 2) / out_last);
      } else {
        out[i] = (int16_t)((val_num - out_last / 2) / out_last);
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
