#include "interface.h"
#include <stdint.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t acc = 0;
    for (size_t i = 0; i < word_count; ++i) {
      acc += (uint64_t)__builtin_popcountll(left[i] & right[i]);
    }
    total = acc;
  }
  return total;
}