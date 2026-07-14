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

typedef struct {
  int left;
  int right;
  float frac;
} InterpParams;

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  if (input_len < 1 || output_len < 1 || iters < 1) return 0;

  // Allocate output buffer once, not per iteration
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  // Precompute all interpolation parameters once to avoid redundant work
  InterpParams* params = (InterpParams*)malloc((size_t)output_len * sizeof(InterpParams));
  if (!params) { free(out); return 0; }

  const double scale = output_len > 1 ? (double)(input_len - 1) / (double)(output_len - 1) : 0.0;
  for (int i = 0; i < output_len; ++i) {
    const double pos = (double)i * scale;
    const int left = (int)pos;
    int right = left + 1;
    if (right >= input_len) right = input_len - 1;
    const float frac = (float)(pos - (double)left);
    params[i] = (InterpParams){.left = left, .right = right, .frac = frac};
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      const InterpParams p = params[i];
      const float sample = (float)src[p.left] * (1.0f - p.frac) + (float)src[p.right] * p.frac;
      out[i] = (int16_t)lroundf(sample);
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(params);
  free(out);
  return hash;
}