#include "interface.h"
#include <stdint.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  uint64_t hash = 0;
  double step = (double)(input_len - 1) / (double)(output_len - 1);
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int i = 0; i < output_len; ++i) {
      double pos = i * step;
      int left = (int)pos;
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      double frac = pos - (double)left;
      double sample = (double)src[left] + ((double)src[right] - (double)src[left]) * frac;
      int16_t out_val = (int16_t)(sample + (sample >= 0.0 ? 0.5 : -0.5));
      hash ^= (uint16_t)out_val;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
