#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const std::size_t n = xs.size();
  if (n < 2 || iters <= 0) return 0;

  int64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t sum = 0;

#pragma omp parallel for schedule(static) reduction(+ : sum)
    for (std::int64_t i = 0; i < static_cast<std::int64_t>(n); ++i) {
      const int32_t xi = xs[static_cast<std::size_t>(i)];
      const int32_t yi = ys[static_cast<std::size_t>(i)];
      int64_t local = 0;

      for (std::size_t j = static_cast<std::size_t>(i) + 1; j < n; ++j) {
        const int32_t dx = xi - xs[j];
        const int32_t dy = yi - ys[j];
        local += (dx >= 0) ? static_cast<int64_t>(dx) : -static_cast<int64_t>(dx);
        local += (dy >= 0) ? static_cast<int64_t>(dy) : -static_cast<int64_t>(dy);
      }

      sum += local;
    }

    total = sum;
  }

  return total;
}
