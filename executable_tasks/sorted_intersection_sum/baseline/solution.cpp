#include "interface.h"

#include <algorithm>

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (uint32_t v : lhs) {
      if (std::binary_search(rhs.begin(), rhs.end(), v)) {
        sum += static_cast<uint64_t>(v);
      }
    }
  }
  return sum;
}
