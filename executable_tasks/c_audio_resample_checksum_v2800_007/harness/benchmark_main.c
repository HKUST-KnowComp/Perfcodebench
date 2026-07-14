#include "interface.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint16_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

static uint64_t expected_checksum(const int16_t* src, int input_len, int output_len) {
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) exit(3);
  double scale = (double)(input_len - 1) / (double)(output_len - 1);
  for (int i = 0; i < output_len; ++i) {
    double pos = (double)i * scale;
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
  uint64_t hash = checksum_i16(out, (size_t)output_len);
  free(out);
  return hash;
}

int main(void) {
  const int input_len = 5799;
  const int output_len = 7850;
  const int iters = 29;
  int16_t* src = (int16_t*)malloc((size_t)input_len * sizeof(int16_t));
  if (!src) return 2;
  uint64_t state = 0xABCDEF1234567890ULL;
  for (int i = 0; i < input_len; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    src[i] = (int16_t)((int32_t)((state >> 32) & 0xFFFFu) - 32768);
  }
  uint64_t expected = expected_checksum(src, input_len, output_len);
  clock_t start = clock();
  uint64_t actual = run(src, input_len, output_len, iters);
  clock_t end = clock();
  long long elapsed_ns = (long long)(((end - start) * 1000000000.0) / CLOCKS_PER_SEC);
  int ok = (actual == expected);
  printf("{\"ok\":%s,\"elapsed_ns\":%lld,\"expected_checksum\":%" PRIu64 ",\"actual_checksum\":%" PRIu64 "}\n",
         ok ? "true" : "false",
         elapsed_ns,
         expected,
         actual);
  free(src);
  return ok ? 0 : 1;
}
