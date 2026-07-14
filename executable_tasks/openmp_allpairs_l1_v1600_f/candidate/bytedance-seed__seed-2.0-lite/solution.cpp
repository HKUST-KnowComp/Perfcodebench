#include "interface.h"
#include <omp.h>
#include <algorithm>
#include <vector>
#include <cstdint>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const std::size_t n = xs.size();
  if (n < 2) return 0;

  std::vector<int32_t> sorted_x = xs;
  std::sort(sorted_x.begin(), sorted_x.end());
  std::vector<int32_t> sorted_y = ys;
  std::sort(sorted_y.begin(), sorted_y.end());

  int64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t sum_x = 0, sum_y = 0;

    #pragma omp parallel for reduction(+:sum_x)
    for (std::size_t k = 0; k < n; ++k) {
      sum_x += static_cast<int64_t>(sorted_x[k]) * (2LL * k - n + 1);
    }

    #pragma omp parallel for reduction(+:sum_y)
    for (std::size_t k = 0; k < n; ++k) {
      sum_y += static_cast<int64_t>(sorted_y[k]) * (2LL * k - n + 1);
    }

    total = sum_x + sum_y;
  }
  return total;
}