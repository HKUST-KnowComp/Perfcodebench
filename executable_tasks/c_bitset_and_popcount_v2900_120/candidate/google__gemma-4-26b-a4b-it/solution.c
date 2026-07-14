#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total_sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (size_t i = 0; i < word_count; ++i) {
      iter_sum += (uint64_t)__builtin_popcountll(left[i] & right[i]);
    }
    total_sum += iter_sum;
  }
  
  return total_sum;
}