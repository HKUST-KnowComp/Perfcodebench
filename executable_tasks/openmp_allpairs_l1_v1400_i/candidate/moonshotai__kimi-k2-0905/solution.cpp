#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs,
                        const std::vector<int32_t>& ys,
                        int iters) {
  const std::size_t n = xs.size();
  if (n < 2) return 0;

  std::vector<int64_t> buf_x(n);
  std::vector<int64_t> buf_y(n);

  int64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t sum_x = 0;
    int64_t sum_y = 0;

    #pragma omp parallel sections reduction(+:sum_x,sum_y)
    {
      #pragma omp section
      {
        std::copy(xs.begin(), xs.end(), buf_x.begin());
        std::sort(buf_x.begin(), buf_x.end());
        int64_t prefix = 0;
        for (std::size_t i = 0; i < n; ++i) {
          sum_x += buf_x[i] * static_cast<int64_t>(i) - prefix;
          prefix += buf_x[i];
        }
      }

      #pragma omp section
      {
        std::copy(ys.begin(), ys.end(), buf_y.begin());
        std::sort(buf_y.begin(), buf_y.end());
        int64_t prefix = 0;
        for (std::size_t i = 0; i < n; ++i) {
          sum_y += buf_y[i] * static_cast<int64_t>(i) - prefix;
          prefix += buf_y[i];
        }
      }
    }

    total = sum_x + sum_y;
  }
  return total;
}