#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint64_t fnv1a_mix_i16(uint64_t hash, int16_t v) {
  hash ^= (uint16_t)v;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  if (iters <= 0) return 0;
  if (src == NULL) return 0;
  if (input_len <= 0 || output_len <= 0) return 0;

  uint64_t hash = 0;

  /* Degenerate source: every output sample is the only input sample. */
  if (input_len == 1) {
    const int16_t v = src[0];
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t h = 1469598103934665603ULL;
      for (int i = 0; i < output_len; ++i) {
        h = fnv1a_mix_i16(h, v);
      }
      hash = h;
    }
    return hash;
  }

  /* Degenerate output: baseline formula would divide by zero; use first sample. */
  if (output_len == 1) {
    const int16_t v = src[0];
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t h = 1469598103934665603ULL;
      h = fnv1a_mix_i16(h, v);
      hash = h;
    }
    return hash;
  }

  const uint64_t denom = (uint64_t)(output_len - 1);
  const uint64_t step = (((uint64_t)(input_len - 1)) << 32) / denom;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    uint64_t pos_fp = 0;

    for (int i = 0; i < output_len; ++i) {
      uint32_t left = (uint32_t)(pos_fp >> 32);
      uint32_t frac_fp = (uint32_t)pos_fp;

      int32_t l = src[left];
      int32_t r;
      if ((int)left + 1 < input_len) {
        r = src[left + 1];
      } else {
        r = src[input_len - 1];
      }

      int64_t diff = (int64_t)r - (int64_t)l;
      int64_t interp = ((int64_t)l << 32) + diff * (int64_t)frac_fp;

      int64_t rounded;
      if (interp >= 0) {
        rounded = (interp + (1LL << 31)) >> 32;
      } else {
        rounded = -(((-interp) + (1LL << 31)) >> 32);
      }

      h = fnv1a_mix_i16(h, (int16_t)rounded);
      pos_fp += step;
    }

    hash = h;
  }

  return hash;
}
