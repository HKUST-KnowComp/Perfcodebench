#include "interface.h"

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t i = 0, j = 0;
    const size_t lhs_size = lhs.size();
    const size_t rhs_size = rhs.size();
    
    while (i < lhs_size && j < rhs_size) {
      uint32_t lhs_val = lhs[i];
      uint32_t rhs_val = rhs[j];
      
      if (lhs_val < rhs_val) {
        ++i;
      } else if (lhs_val > rhs_val) {
        ++j;
      } else {
        sum += static_cast<uint64_t>(lhs_val);
        ++i;
        ++j;
      }
    }
  }
  return sum;
}