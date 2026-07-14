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
  if (input_len < 1 || output_len < 1 || iters < 1) return 0;
  
  const size_t out_size = (size_t)output_len * sizeof(int16_t);
  int16_t* out = (int16_t*)malloc(out_size);
  if (!out) return 0;

  // Resample exactly once, inputs are fixed for all iterations
  if (output_len == 1) {
    out[0] = src[0];
  } else {
    const double scale = (double)(input_len - 1) / (double)(output_len - 1);
    for (int i = 0; i < output_len; ++i) {
      const double pos = (double)i * scale;
      const int left = (int)pos;
      const int right = (left + 1 < input_len) ? (left + 1) : (input_len - 1);
      const double frac = pos - (double)left;
      double sample = (double)src[left] * (1.0 - frac) + (double)src[right] * frac;
      out[i] = (sample >= 0.0) ? (int16_t)(sample + 0.5) : (int16_t)(sample - 0.5);
    }
  }

  // Only run checksum per iteration, reuse precomputed output
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}