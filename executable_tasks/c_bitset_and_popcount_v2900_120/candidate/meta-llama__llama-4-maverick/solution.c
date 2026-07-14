#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

#if defined(__GNUC__) || defined(__clang__)
#include <x86intrin.h>
#endif

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_total = 0;
    for (size_t i = 0; i < word_count; ++i) {
      uint64_t word = left[i] & right[i];
#if defined(__GNUC__) || defined(__clang__)
      iter_total += _mm_popcnt_u64(word);
#else
      for (int b = 0; b < 64; ++b) {
        iter_total += (uint64_t)((word >> b) & 1ULL);
      }
#endif
    }
    total = iter_total;
  }
  return total;
}
