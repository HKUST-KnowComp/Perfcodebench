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
  if (output_len <= 0) return 0;
  if (input_len <= 1) {
    // Handle edge case where input_len - 1 would be 0
    // The original code would result in NaN/Inf via division by zero if output_len > 1
    // However, following the original logic's structure:
    int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;
    for (int i = 0; i < output_len; ++i) out[i] = src[0];
    uint64_t hash = checksum_i16(out, (size_t)output_len);
    free(out);
    return hash;
  }

  uint64_t hash = 0;
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  // Pre-calculate constants for fixed-point math
  // We want to represent 'pos' as a fixed-point number.
  // pos = i * (input_len - 1) / (output_len - 1)
  // Let's use a large scale factor to maintain precision equivalent to double.
  // The original uses double precision. To match exactly, we must be careful.
  // However, the original code uses: double sample = (double)src[left] * (1.0 - frac) + (double)src[right] * frac;
  // which is equivalent to: sample = src[left] + frac * (src[right] - src[left])
  
  const double step = (double)(input_len - 1) / (double)(output_len - 1);

  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      double pos = (double)i * step;
      int left = (int)pos;
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      
      double frac = pos - (double)left;
      double sample = (double)src[left] * (1.0 - frac) + (double)src[right] * frac;
      
      if (sample >= 0.0) {
        out[i] = (int16_t)(sample + 0.5);
      } else {
        out[i] = (int16_t)(sample - 0.5);
      }
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(out);
  return hash;
}

// Note: To ensure exact correctness with the original double-based logic, 
// we must use double or extremely high precision fixed-point. 
// The primary performance gain here is moving malloc/free out of the loop.
// To further optimize, we use the double 'step' to avoid repeated division.
