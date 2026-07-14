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
  if (iters <= 0) return 0;

  // Allocate output buffer once to eliminate per-iteration malloc/free overhead
  const size_t out_bytes = (size_t)output_len * sizeof(int16_t);
  int16_t* out = (int16_t*)malloc(out_bytes);
  if (!out) return 0;

  // Precompute resampling coefficients once, reused across all iterations
  typedef struct {
    int left_idx;
    int right_idx;
    double w_left;
    double w_right;
  } ResampleCoeff;
  ResampleCoeff* coeffs = (ResampleCoeff*)malloc((size_t)output_len * sizeof(ResampleCoeff));
  if (!coeffs) {
    free(out);
    return 0;
  }

  const double scale = (double)(input_len - 1) / (double)(output_len - 1);
  for (int i = 0; i < output_len; ++i) {
    const double pos = (double)i * scale;
    const int left = (int)pos;
    int right = left + 1;
    if (right >= input_len) right = input_len - 1;
    const double frac = pos - (double)left;
    coeffs[i].left_idx = left;
    coeffs[i].right_idx = right;
    coeffs[i].w_left = 1.0 - frac;
    coeffs[i].w_right = frac;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Tight hot inner loop with no redundant calculations
    for (int i = 0; i < output_len; ++i) {
      const ResampleCoeff c = coeffs[i];
      const double sample = (double)src[c.left_idx] * c.w_left + (double)src[c.right_idx] * c.w_right;
      int16_t s;
      if (sample >= 0.0) {
        s = (int16_t)(sample + 0.5);
      } else {
        s = (int16_t)(sample - 0.5);
      }
      out[i] = s;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  // Single cleanup pass
  free(coeffs);
  free(out);
  return hash;
}