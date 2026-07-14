#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  // Precompute constants
  const int in_last = input_len - 1;
  const int out_last = output_len - 1;
  const double step = (double)in_last / (double)out_last;

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = FNV_OFFSET;

    for (int i = 0; i < output_len; ++i) {
      double pos = (double)i * step;
      int left = (int)pos;
      int right = left + 1;
      if (right >= input_len) right = in_last;

      double a = (double)src[left];
      double b = (double)src[right];
      double frac = pos - (double)left;
      double sample = a + (b - a) * frac;

      int16_t outv;
      if (sample >= 0.0) {
        outv = (int16_t)(sample + 0.5);
      } else {
        outv = (int16_t)(sample - 0.5);
      }

      h ^= (uint16_t)outv;
      h *= FNV_PRIME;
    }

    hash = h; // Match baseline behavior: hash of the last iteration's output
  }

  return hash;
}
