#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t count = 0;
  const std::size_t n = left.size();
  const uint64_t* __restrict__ l = left.data();
  const uint64_t* __restrict__ r = right.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current = 0;
    std::size_t i = 0;
    // Unroll by 4 to improve ILP and hide popcnt latency
    for (; i + 3 < n; i += 4) {
      current += __builtin_popcountll(l[i] & r[i]);
      current += __builtin_popcountll(l[i+1] & r[i+1]);
      current += __builtin_popcountll(l[i+2] & r[i+2]);
      current += __builtin_popcountll(l[i+3] & r[i+3]);
    }
    for (; i < n; ++i) {
      current += __builtin_popcountll(l[i] & r[i]);
    }
    count = current;
  }
  return count;
}
