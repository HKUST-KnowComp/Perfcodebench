#include "interface.h"

#include <cstdint>

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    auto a = lhs.begin();
    auto b = rhs.begin();
    while (a != lhs.end() && b != rhs.end()) {
      if (*a < *b) {
        ++a;
      } else if (*b < *a) {
        ++b;
      } else {
        sum += *a;
        ++a;
        ++b;
      }
    }
  }
  return sum;
}
