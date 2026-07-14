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
    uint64_t hash = checksum_i16(out, (size_t)output_len);
    free(out);
    return hash;
  }

  const int in_last = input_len - 1;
  const int out_last = output_len - 1;

  for (int i = 0; i < output_len; ++i) {
    long long num = (long long)i * (long long)in_last;
    int left = (int)(num / out_last);
    int rem = (int)(num - (long long)left * out_last);
    int right = left + 1;
    if (right >= input_len) right = input_len - 1;

    int32_t a = (int32_t)src[left];
    int32_t b = (int32_t)src[right];
    int32_t diff = b - a;

    long long acc = (long long)a * out_last + (long long)diff * rem;
    if (acc >= 0) {
      out[i] = (int16_t)((acc + out_last / 2) / out_last);
    } else {
      out[i] = (int16_t)((acc - out_last / 2) / out_last);
    }
  }

  uint64_t hash = checksum_i16(out, (size_t)output_len);
  free(out);
  return hash;
}
