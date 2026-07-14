#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  if (iters <= 0) return 0;
  std::size_t n = xs.size();
  int64_t sum = 0;
  #pragma omp parallel for reduction(+:sum) schedule(static)
  for (std::size_t i = 0; i < n; ++i) {
    int32_t xi = xs[i];
    int32_t yi = ys[i];
    int64_t local = 0;
    for (std::size_t j = i + 1; j < n; ++j) {
      local += std::abs(xi - xs[j]) + std::abs(yi - ys[j]);
    }
    sum += local;
  }
  return sum;
}
