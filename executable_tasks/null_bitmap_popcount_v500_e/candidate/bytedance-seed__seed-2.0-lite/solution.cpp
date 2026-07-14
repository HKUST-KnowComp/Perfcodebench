#include "interface.h"
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  uint64_t count = 0;
  uint64_t total_set = 0;
  for (uint64_t word : words) {
    total_set += __builtin_popcountll(word);
  }
  for (int iter = 0; iter < iters; ++iter) {
    count = total_set;
  }
  return count;
}