#include "interface.h"

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  const size_t n = left.size();
  uint64_t result = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t count = 0;
    for (size_t i = 0; i < n; ++i) {
      count += __builtin_popcountll(left[i] & right[i]);
    }
    result = count;
  }
  return result;
}
