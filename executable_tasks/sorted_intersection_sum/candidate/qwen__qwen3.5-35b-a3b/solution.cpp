#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t i = 0;
    size_t j = 0;
    const size_t n = lhs.size();
    const size_t m = rhs.size();

    while (i < n && j < m) {
      if (lhs[i] < rhs[j]) {
        ++i;
      } else if (lhs[i] > rhs[j]) {
        ++j;
      } else {
        sum += static_cast<uint64_t>(lhs[i]);
        ++i;
        ++j;
      }
    }
  }
  return sum;
}