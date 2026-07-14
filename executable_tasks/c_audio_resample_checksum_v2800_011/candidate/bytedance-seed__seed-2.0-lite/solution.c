#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint16_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  uint64_t hash = 0;
  if (input_len < 1 || output_len < 1) return 0;
  const double scale = (double)(input_len - 1) / (double)(output_len - 1);
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      const double pos = (double)i * scale;
      const int left = (int)pos;
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      const double frac = pos - (double)left;
      const double sample = (double)src[left] * (1.0 - frac) + (double)src[right] * frac;
      out[i] = (int16_t)rint(sample);
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}