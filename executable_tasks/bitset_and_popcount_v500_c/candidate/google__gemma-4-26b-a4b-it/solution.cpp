#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t total_count = 0;
  const std::size_t n = left.size();
  const uint64_t* l_ptr = left.data();
  const uint64_t* r_ptr = right.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_count = 0;
    for (std::size_t i = 0; i < n; ++i) {
      iter_count += static_cast<uint64_t>(__builtin_popcountll(l_ptr[i] & r_ptr[i]));
    }
    total_count = iter_count;
  }
  return total_count;
}