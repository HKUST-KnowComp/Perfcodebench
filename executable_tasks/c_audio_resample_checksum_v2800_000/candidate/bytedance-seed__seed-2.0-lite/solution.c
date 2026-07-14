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
  double frac;
} ResamplePoint;

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  if (iters <= 0 || input_len <= 0 || output_len <= 0) return 0;

  // Allocate output buffer once, reused across all iterations
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  // Precompute all resampling parameters once to avoid redundant calculations
  ResamplePoint* points = (ResamplePoint*)malloc((size_t)output_len * sizeof(ResamplePoint));
  if (!points) {
    free(out);
    return 0;
  }

  const double scale = (double)(input_len - 1) / (double)(output_len - 1);
  for (int i = 0; i < output_len; ++i) {
    double pos = (double)i * scale;
    int left = (int)pos;
    int right = left + 1;
    if (right >= input_len) right = input_len - 1;
    double frac = pos - (double)left;
    points[i].left = left;
    points[i].right = right;
    points[i].frac = frac;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Hot path: only interpolation and checksum, no expensive allocations or calculations
    for (int i = 0; i < output_len; ++i) {
      const ResamplePoint p = points[i];
      double sample = (double)src[p.left] * (1.0 - p.frac) + (double)src[p.right] * p.frac;
      if (sample >= 0.0) {
        out[i] = (int16_t)(sample + 0.5);
      } else {
        out[i] = (int16_t)(sample - 0.5);
      }
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  // Cleanup allocated resources once after all iterations
  free(points);
  free(out);
  return hash;
}