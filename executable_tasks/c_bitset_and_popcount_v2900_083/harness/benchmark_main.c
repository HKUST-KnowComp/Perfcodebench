#include "interface.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static uint64_t expected_count(const uint64_t* left, const uint64_t* right, size_t word_count) {
  uint64_t total = 0;
  for (size_t i = 0; i < word_count; ++i) {
    total += (uint64_t)__builtin_popcountll(left[i] & right[i]);
  }
  return total;
}

int main(void) {
  const size_t word_count = 143100u;
  const int iters = 16;
  uint64_t* left = (uint64_t*)malloc(word_count * sizeof(uint64_t));
  uint64_t* right = (uint64_t*)malloc(word_count * sizeof(uint64_t));
  if (!left || !right) return 2;
  uint64_t state = 0xDEADBEEF12345678ULL;
  for (size_t i = 0; i < word_count; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    left[i] = state ^ (state << 7);
    state = state * 2862933555777941757ULL + 3037000493ULL;
    right[i] = state ^ (state >> 11);
  }
  uint64_t expected = expected_count(left, right, word_count);
  clock_t start = clock();
  uint64_t actual = run(left, right, word_count, iters);
  clock_t end = clock();
  long long elapsed_ns = (long long)(((end - start) * 1000000000.0) / CLOCKS_PER_SEC);
  int ok = (actual == expected);
  printf("{\"ok\":%s,\"elapsed_ns\":%lld,\"expected_checksum\":%" PRIu64 ",\"actual_checksum\":%" PRIu64 "}\n",
         ok ? "true" : "false", elapsed_ns, expected, actual);
  free(left);
  free(right);
  return ok ? 0 : 1;
}
