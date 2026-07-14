#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_count = 0;
    for (uint64_t word : words) {
      current_count += __builtin_popcountll(word);
    }
    count = current_count;
  }
  return count;
}