#include "interface.h"

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;
    std::size_t j = 0;
    while (i < lhs.size() && j < rhs.size()) {
      if (lhs[i] == rhs[j]) {
        sum += static_cast<uint64_t>(lhs[i]);
        ++i;
        ++j;
      } else if (lhs[i] < rhs[j]) {
        ++i;
      } else {
        ++j;
      }
    }
  }
  return sum;
}
