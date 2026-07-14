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

  if (output_len == 1 || input_len == 1) {
    int16_t v = src[0];
    for (int i = 0; i < output_len; ++i) out[i] = v;
  } else {
    const int in_last = input_len - 1;
    const int out_last = output_len - 1;

    int left = 0;
    uint64_t rem = 0;
    const uint64_t den = (uint64_t)out_last;
    const uint64_t num_step = (uint64_t)in_last;

    for (int i = 0; i < output_len; ++i) {
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;

      const int16_t s0 = src[left];
      const int16_t s1 = src[right];
      const int32_t diff = (int32_t)s1 - (int32_t)s0;

      int32_t sample_num = (int32_t)s0 * (int32_t)out_last + diff * (int32_t)rem;
      if (sample_num >= 0) {
        sample_num += (int32_t)(den / 2);
      } else {
        sample_num -= (int32_t)(den / 2);
      }
      out[i] = (int16_t)(sample_num / (int32_t)den);

      rem += num_step;
      if (rem >= den) {
        uint64_t adv = rem / den;
        rem -= adv * den;
        left += (int)adv;
        if (left > in_last) left = in_last;
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
