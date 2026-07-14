#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static inline uint64_t checksum_i16(const int16_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint16_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

typedef struct {
  int left;
  float frac;
} InterpParams;

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  if (input_len < 1 || output_len < 1 || iters < 1) return 0;

  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  InterpParams* interp = (InterpParams*)malloc((size_t)output_len * sizeof(InterpParams));
  if (!interp) { free(out); return 0; }

  const float scale = input_len == 1 ? 0.0f : (float)(input_len - 1) / (float)(output_len - 1);
  for (int i = 0; i < output_len; ++i) {
    if (input_len == 1) {
      interp[i].left = 0;
      interp[i].frac = 0.0f;
    } else {
      const float pos = (float)i * scale;
      int left = (int)pos;
      float frac = pos - (float)left;
      if (left >= input_len - 1) {
        interp[i].left = input_len - 2;
        interp[i].frac = 1.0f;
      } else {
        interp[i].left = left;
        interp[i].frac = frac;
      }
    }
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      const InterpParams p = interp[i];
      const float s0 = (float)src[p.left];
      const float s1 = (float)src[p.left + 1];
      float sample = s0 * (1.0f - p.frac) + s1 * p.frac;
      if (sample >= 0.0f) {
        out[i] = (int16_t)(sample + 0.5f);
      } else {
        out[i] = (int16_t)(sample - 0.5f);
      }
    }
    hash = checksum_i16(out, (size_t)output_len);
  }

  free(interp);
  free(out);
  return hash;
}