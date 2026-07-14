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
  double step = (double)(input_len - 1) / (double)(output_len - 1);
  size_t out_size = (size_t)output_len;
  int16_t* out = (int16_t*)malloc(out_size * sizeof(int16_t));
  if (!out) return 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      double pos = i * step;
      int left = (int)pos;
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      double frac = pos - (double)left;
      double sample = (double)src[left] * (1.0 - frac) + (double)src[right] * frac;
      if (sample >= 0.0) {
        out[i] = (int16_t)(sample + 0.5);
      } else {
        out[i] = (int16_t)(sample - 0.5);
      }
    }
  }
  uint64_t hash = checksum_i16(out, out_size);
  free(out);
  return hash;
}
