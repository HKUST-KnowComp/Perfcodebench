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
  (void)iters;
  if (output_len <= 0) return 0;

  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  if (output_len == 1) {
    out[0] = src[0];
  } else {
    int N = input_len - 1;
    int M = output_len - 1;
    int left = 0;
    int rem = 0;

    for (int i = 0; i < output_len; ++i) {
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      
      int diff = src[right] - src[left];
      int64_t num = (int64_t)src[left] * M + (int64_t)diff * rem;
      int64_t rounded = (num >= 0) ? (num + M/2) / M : (num - M/2) / M;
      out[i] = (int16_t)rounded;

      rem += N;
      if (rem >= M) {
        rem -= M;
        left++;
      }
    }
  }

  uint64_t hash = checksum_i16(out, (size_t)output_len);
  free(out);
  return hash;
}
