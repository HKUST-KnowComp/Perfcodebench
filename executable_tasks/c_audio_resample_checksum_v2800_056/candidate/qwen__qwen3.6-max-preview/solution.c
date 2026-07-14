#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

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

  double step = (output_len > 1) ? (double)(input_len - 1) / (double)(output_len - 1) : 0.0;
  uint64_t hash = 0;
  int limit = output_len - 1;

  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < limit; ++i) {
      double pos = i * step;
      int left = (int)pos;
      double frac = pos - left;
      double sample = src[left] + frac * (src[left + 1] - src[left]);
      out[i] = (int16_t)(sample + (sample >= 0.0 ? 0.5 : -0.5));
    }
    out[limit] = src[input_len - 1];
    hash = checksum_i16(out, (size_t)output_len);
  }
  
  free(out);
  return hash;
}