#include "interface.h"

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  if (iters <= 0) return 0;
  uint64_t total_set = 0;
  for (uint64_t word : words) {
    total_set += __builtin_popcountll(word);
  }
  return total_set * static_cast<uint64_t>(iters);
}