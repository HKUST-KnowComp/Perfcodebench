#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

#if defined(__GNUC__) || defined(__clang__)
#define POPCOUNT(x) __builtin_popcountll(x)
#else
#define POPCOUNT(x) popcount(x)
#endif

static uint64_t popcount(uint64_t x) {
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
    x = x + (x >> 8);
    x = x + (x >> 16);
    x = x + (x >> 32);
    return x & 0x7f;
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < word_count; ++i) {
      uint64_t word = left[i] & right[i];
      total += POPCOUNT(word);
    }
  }
  return total;
}