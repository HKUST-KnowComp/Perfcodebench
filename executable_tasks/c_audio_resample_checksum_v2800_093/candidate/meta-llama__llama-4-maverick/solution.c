#include "interface.h"

#include <stdint.h>

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
  double scale = (double)(input_len - 1) / (double)(output_len - 1);
  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      double pos = (double)i * scale;
      int left = (int)pos;
      double frac = pos - (double)left;
      int16_t s_left = src[left];
      int16_t s_right = src[left + (left < input_len - 1)];
      double sample = (double)s_left + frac * ((double)s_right - (double)s_left);
      out[i] = (int16_t)(sample + (sample >= 0.0 ? 0.5 : -0.5));
    }
    hash = checksum_i16(out, (size_t)output_len);
  }
  free(out);
  return hash;
}
