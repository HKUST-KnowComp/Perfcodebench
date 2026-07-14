#include "interface.h"

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    auto lhs_it = lhs.begin();
    auto rhs_it = rhs.begin();
    while (lhs_it != lhs.end() && rhs_it != rhs.end()) {
      if (*lhs_it < *rhs_it) {
        ++lhs_it;
      } else if (*rhs_it < *lhs_it) {
        ++rhs_it;
      } else {
        sum += static_cast<uint64_t>(*lhs_it);
        ++lhs_it;
        ++rhs_it;
      }
    }
  }
  return sum;
}