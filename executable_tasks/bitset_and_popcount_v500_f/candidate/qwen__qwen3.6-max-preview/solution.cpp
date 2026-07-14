#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t count = 0;
  const size_t n = left.size();
  const uint64_t* l = left.data();
  const uint64_t* r = right.data();

  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    size_t i = 0;
    // Unroll by 4 to improve ILP and hide popcnt latency
    for (; i + 3 < n; i += 4) {
      count += __builtin_popcountll(l[i] & r[i]);
      count += __builtin_popcountll(l[i+1] & r[i+1]);
      count += __builtin_popcountll(l[i+2] & r[i+2]);
      count += __builtin_popcountll(l[i+3] & r[i+3]);
    }
    for (; i < n; ++i) {
      count += __builtin_popcountll(l[i] & r[i]);
    }
  }
  return count;
}
