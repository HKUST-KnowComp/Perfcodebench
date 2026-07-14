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
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;
  uint64_t hash = 0;
  double step = (double)(input_len - 1) / (double)(output_len - 1);
  for (int iter = 0; iter < iters; ++iter) {
    double pos = 0.0;
    int left = 0;
    for (int i = 0; i < output_len; ++i) {
      int right = (int)(pos + 1.0) >= input_len ? input_len - 1 : left + 1;
      double frac = pos - (double)left;
      double sample = (double)src[left] + frac * ((double)src[right] - (double)src[left]);
      out[i] = sample >= 0.0 ? (int16_t)(sample + 0.5) : (int16_t)(sample - 0.5);
      pos += step;
      left = (int)pos;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }
  free(out);
  return hash;
}
