#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline int64_t abs_diff_i32(int32_t a, int32_t b) {
  return (a >= b) ? static_cast<int64_t>(a) - static_cast<int64_t>(b)
                  : static_cast<int64_t>(b) - static_cast<int64_t>(a);
}

}  // namespace

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const std::size_t n = xs.size();
  if (n < 2 || iters <= 0) return 0;

  int64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t sum = 0;

#pragma omp parallel for schedule(static) reduction(+:sum)
    for (std::int64_t i = 0; i < static_cast<std::int64_t>(n); ++i) {
      const int32_t xi = xs[static_cast<std::size_t>(i)];
      const int32_t yi = ys[static_cast<std::size_t>(i)];
      int64_t local = 0;

      for (std::size_t j = static_cast<std::size_t>(i) + 1; j < n; ++j) {
        local += abs_diff_i32(xi, xs[j]) + abs_diff_i32(yi, ys[j]);
      }

      sum += local;
    }

    total = sum;
  }

  return total;
}
