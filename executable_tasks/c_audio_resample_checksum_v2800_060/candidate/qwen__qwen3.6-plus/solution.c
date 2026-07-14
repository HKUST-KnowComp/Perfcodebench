#include "interface.h"
#include <stdint.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  uint64_t hash = 0;
  double step = (double)(input_len - 1) / (double)(output_len - 1);
  int last_idx = input_len - 1;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (int i = 0; i < output_len; ++i) {
      double pos = (double)i * step;
      int left = (int)pos;
      int right = left + 1;
      if (right > last_idx) right = last_idx;
      double frac = pos - (double)left;
      double sample = src[left] + (src[right] - src[left]) * frac;
      int16_t val = (int16_t)(sample + (sample >= 0.0 ? 0.5 : -0.5));
      h ^= (uint16_t)val;
      h *= 1099511628211ULL;
    }
    hash = h;
  }
  return hash;
}