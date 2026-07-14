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
  typedef struct {
    int left;
    int right;
    double w0;
    double w1;
  } InterpParams;

  InterpParams* params = (InterpParams*)malloc((size_t)output_len * sizeof(InterpParams));
  if (!params) return 0;

  const double in_scale = (double)(input_len - 1) / (double)(output_len - 1);
  for (int i = 0; i < output_len; ++i) {
    double pos = (double)i * in_scale;
    int left = (int)pos;
    int right = left + 1;
    if (right >= input_len) right = input_len - 1;
    double frac = pos - (double)left;
    params[i].left = left;
    params[i].right = right;
    params[i].w0 = 1.0 - frac;
    params[i].w1 = frac;
  }

  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) {
    free(params);
    return 0;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      const InterpParams* p = &params[i];
      double sample = (double)src[p->left] * p->w0 + (double)src[p->right] * p->w1;
      if (sample >= 0.0) {
        out[i] = (int16_t)(sample + 0.5);
      } else {
        out[i] = (int16_t)(sample - 0.5);
      }
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  free(params);
  return hash;
}