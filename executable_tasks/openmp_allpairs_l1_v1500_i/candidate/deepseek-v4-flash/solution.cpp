#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  std::size_t n = xs.size();
  int64_t sum = 0;
  #pragma omp parallel for schedule(dynamic, 1) reduction(+:sum)
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = i + 1; j < n; ++j) {
      sum += static_cast<int64_t>(std::abs(xs[i] - xs[j])) +
             static_cast<int64_t>(std::abs(ys[i] - ys[j]));
    }
  }
  return sum * iters;
}
