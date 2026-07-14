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

  if (output_len == 1 || input_len == 1) {
    const int16_t v = src[0];
    for (int i = 0; i < output_len; ++i) out[i] = v;
  } else {
    const int in_last = input_len - 1;
    const int out_last = output_len - 1;

    int left = 0;
    int64_t acc = 0;
    const int64_t step = ((int64_t)in_last << 32) / (int64_t)out_last;

    for (int i = 0; i < output_len; ++i) {
      if (i == output_len - 1) {
        out[i] = src[in_last];
        break;
      }

      const int64_t next_acc = acc + step;
      const int next_left = (int)(next_acc >> 32);
      const uint32_t frac = (uint32_t)acc;

      const int16_t s0 = src[left];
      const int16_t s1 = src[left + 1];
      const int32_t diff = (int32_t)s1 - (int32_t)s0;

      int64_t interp = ((int64_t)diff * (int64_t)frac + (1LL << 31)) >> 32;
      int32_t sample = (int32_t)s0 + (int32_t)interp;
      out[i] = (int16_t)sample;

      acc = next_acc;
      left = next_left;
    }
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}
