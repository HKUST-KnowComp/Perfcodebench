#include "interface.h"

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t total_count = 0;
  const std::size_t n = left.size();
  const uint64_t* l_data = left.data();
  const uint64_t* r_data = right.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t c0 = 0, c1 = 0, c2 = 0, c3 = 0;
    std::size_t i = 0;
    for (; i + 3 < n; i += 4) {
      c0 += __builtin_popcountll(l_data[i] & r_data[i]);
      c1 += __builtin_popcountll(l_data[i+1] & r_data[i+1]);
      c2 += __builtin_popcountll(l_data[i+2] & r_data[i+2]);
      c3 += __builtin_popcountll(l_data[i+3] & r_data[i+3]);
    }
    uint64_t iter_count = c0 + c1 + c2 + c3;
    for (; i < n; ++i) {
      iter_count += __builtin_popcountll(l_data[i] & r_data[i]);
    }
    total_count += iter_count;
  }
  return total_count;
}