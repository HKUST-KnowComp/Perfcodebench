#include "interface.h"

#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_count = 0;
    for (std::size_t i = 0; i < left.size(); ++i) {
      const uint64_t word = left[i] & right[i];
      iter_count += __builtin_popcountll(word);
    }
    count += iter_count;
  }
  return count;
}