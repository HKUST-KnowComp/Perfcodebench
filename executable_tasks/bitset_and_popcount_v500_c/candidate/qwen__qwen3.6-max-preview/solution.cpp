#include "interface.h"
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t count = 0;
  const uint64_t* l = left.data();
  const uint64_t* r = right.data();
  std::size_t n = left.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_count = 0;
    std::size_t i = 0;
    for (; i + 3 < n; i += 4) {
      current_count += __builtin_popcountll(l[i] & r[i]);
      current_count += __builtin_popcountll(l[i+1] & r[i+1]);
      current_count += __builtin_popcountll(l[i+2] & r[i+2]);
      current_count += __builtin_popcountll(l[i+3] & r[i+3]);
    }
    for (; i < n; ++i) {
      current_count += __builtin_popcountll(l[i] & r[i]);
    }
    count = current_count;
  }
  return count;
}
