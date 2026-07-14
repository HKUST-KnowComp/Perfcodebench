#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int iters) {
  const uint32_t* a = lhs.data();
  const uint32_t* b = rhs.data();
  const size_t na = lhs.size();
  const size_t nb = rhs.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    size_t i = 0;
    size_t j = 0;

    while (i < na && j < nb) {
      const uint32_t av = a[i];
      const uint32_t bv = b[j];
      if (av < bv) {
        ++i;
      } else if (bv < av) {
        ++j;
      } else {
        local_sum += static_cast<uint64_t>(av);
        ++i;
        ++j;
      }
    }

    sum = local_sum;
  }
  return sum;
}
