#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  if (output_len <= 0) return 0;

  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  double num = (double)(input_len - 1);
  double den = (double)(output_len - 1);

  for (int i = 0; i < output_len; ++i) {
    double pos = (double)i * num / den;
    int left = (int)pos;
    int right = left + 1;
    if (right >= input_len) right = input_len - 1;
    double frac = pos - (double)left;
    double sample = (double)src[left] * (1.0 - frac) + (double)src[right] * frac;
    out[i] = (int16_t)(sample + (sample >= 0.0 ? 0.5 : -0.5));
  }

  uint64_t hash = 1469598103934665603ULL;
  for (int i = 0; i < output_len; ++i) {
    hash ^= (uint16_t)out[i];
    hash *= 1099511628211ULL;
  }

  free(out);
  (void)iters;
  return hash;
}