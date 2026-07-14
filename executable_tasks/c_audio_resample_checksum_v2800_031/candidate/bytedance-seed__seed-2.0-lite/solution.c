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

typedef struct {
    int left;
    int right;
    uint16_t frac_scaled;
} InterpParam;

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  if (input_len <= 0 || output_len <= 0 || iters <=0) return 0;
  
  // Allocate output buffer once, not per iteration
  const size_t out_bytes = (size_t)output_len * sizeof(int16_t);
  int16_t* out = (int16_t*)malloc(out_bytes);
  if (!out) return 0;

  // Precompute interpolation parameters once, reuse across all iterations
  InterpParam* params = (InterpParam*)malloc((size_t)output_len * sizeof(InterpParam));
  if (!params) { free(out); return 0; }

  if (output_len == 1) {
    // Edge case matching original behavior for single output sample
    params[0].left = 0;
    params[0].right = input_len - 1;
    params[0].frac_scaled = 0;
  } else {
    const double scale = (double)(input_len - 1) / (double)(output_len - 1);
    for (int i = 0; i < output_len; ++i) {
      const double pos = (double)i * scale;
      const int left = (int)pos;
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      const double frac = pos - (double)left;
      params[i].left = left;
      params[i].right = right;
      params[i].frac_scaled = (uint16_t)(frac * 32768.0 + 0.5);
    }
  }

  uint64_t hash = 0;
  // Hot iteration loop, all expensive work precomputed
  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      const InterpParam p = params[i];
      const int32_t left_samp = src[p.left];
      const int32_t right_samp = src[p.right];
      const int32_t sum = left_samp * (32768 - p.frac_scaled) + right_samp * p.frac_scaled;
      
      // Exact matching rounding to original implementation
      if (sum >= 0) {
        out[i] = (int16_t)((sum + 16384) >> 15);
      } else {
        out[i] = (int16_t)((sum - 16384) >> 15);
      }
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(params);
  free(out);
  return hash;
}