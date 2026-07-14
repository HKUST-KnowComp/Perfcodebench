#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t total_count = 0;
  const size_t size = left.size();
  const uint64_t* l_ptr = left.data();
  const uint64_t* r_ptr = right.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_count = 0;
    for (size_t i = 0; i < size; ++i) {
      current_iter_count += __builtin_popcountll(l_ptr[i] & r_ptr[i]);
    }
    total_count = current_iter_count;
  }
  return total_count;
}