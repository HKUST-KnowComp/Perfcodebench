#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const std::size_t n = xs.size();
  if (iters <= 0 || n < 2) {
    return 0;
  }

  // Copy and sort coordinates
  std::vector<int32_t> sx(xs.begin(), xs.end());
  std::vector<int32_t> sy(ys.begin(), ys.end());
  std::sort(sx.begin(), sx.end());
  std::sort(sy.begin(), sy.end());

  const int64_t nn = static_cast<int64_t>(n);
  int64_t sum = 0;

  // Parallel accumulation of the weighted sums for both coordinates
  #pragma omp parallel for reduction(+:sum) schedule(static)
  for (std::ptrdiff_t k = 0; k < static_cast<std::ptrdiff_t>(n); ++k) {
    int64_t weight = ((static_cast<int64_t>(k) << 1) - nn + 1);
    sum += weight * static_cast<int64_t>(sx[k]);
    sum += weight * static_cast<int64_t>(sy[k]);
  }

  // Preserve semantics: repeat assignment per iteration
  int64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    total = sum;
  }
  return total;
}