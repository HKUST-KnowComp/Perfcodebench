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
  if (input_len <= 0 || output_len <= 0 || iters <= 0) return 0;
  uint64_t hash = 0;
  
  // Allocate output buffer once instead of per iteration to eliminate repeated heap overhead
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  // Edge case: single input sample, all output samples match input
  if (input_len == 1) {
    const int16_t sample = src[0];
    for (int i = 0; i < output_len; ++i) out[i] = sample;
    for (int iter = 0; iter < iters; ++iter) {
      hash = checksum_i16(out, (size_t)output_len);
    }
    free(out);
    return hash;
  }

  // Edge case: single output sample, matches final input sample
  if (output_len == 1) {
    out[0] = src[input_len - 1];
    for (int iter = 0; iter < iters; ++iter) {
      hash = checksum_i16(out, 1);
    }
    free(out);
    return hash;
  }

  // Precompute resampling scale once to avoid repeated calculation
  const float scale = (float)(input_len - 1) / (float)(output_len - 1);

  // Main iteration loop
  for (int iter = 0; iter < iters; ++iter) {
    float pos = 0.0f;
    for (int i = 0; i < output_len; ++i) {
      const int left = (int)pos;
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      const float frac = pos - left;
      const float s0 = (float)src[left];
      const float s1 = (float)src[right];
      // Rearranged interpolation to use 1 multiply instead of 2, same mathematical result
      const float sample = s0 + (s1 - s0) * frac;
      out[i] = (int16_t)roundf(sample); // Matches original rounding behavior exactly
      pos += scale; // Increment position instead of recalculating i*scale, reduces inner loop work
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}