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
  if (output_len <= 0 || input_len <= 0) return 0;
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  const int input_minus_1 = input_len - 1;
  const int output_minus_1 = output_len - 1;
  const double step = (double)input_minus_1 / (double)output_minus_1;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    double current_pos = 0.0;
    for (int i = 0; i < output_len; ++i) {
      const int left = (int)current_pos;
      int right = left + 1;
      if (right >= input_len) right = input_minus_1;
      const double frac = current_pos - (double)left;
      const double sample = (double)src[left] * (1.0 - frac) + (double)src[right] * frac;
      out[i] = (int16_t)round(sample);
      current_pos += step;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}