#include "interface.h"

#include <cstdint>
#include <vector>
#include <immintrinintrin.h>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    for (uint64_t word : words) {
      count += _mm_popcnt_u64(word);
    }
  }
  return count;
}