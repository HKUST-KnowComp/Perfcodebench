#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int /*iters*/) {
  const std::size_t n = xs.size();
  if (n <= 1) return 0;

  // Copy and sort coordinates (in parallel for x and y)
  std::vector<int32_t> sx(xs.begin(), xs.end());
  std::vector<int32_t> sy(ys.begin(), ys.end());

  #pragma omp parallel sections
  {
    #pragma omp section
    {
      std::sort(sx.begin(), sx.end());
    }
    #pragma omp section
    {
      std::sort(sy.begin(), sy.end());
    }
  }

  int64_t sumx = 0;
  int64_t sumy = 0;
  const int64_t ln = static_cast<int64_t>(n);
  for (std::size_t i = 0; i < n; ++i) {
    const int64_t coeff = static_cast<int64_t>(2 * static_cast<int64_t>(i)) - ln + 1;
    sumx += static_cast<int64_t>(sx[i]) * coeff;
    sumy += static_cast<int64_t>(sy[i]) * coeff;
  }

  return sumx + sumy;
}
