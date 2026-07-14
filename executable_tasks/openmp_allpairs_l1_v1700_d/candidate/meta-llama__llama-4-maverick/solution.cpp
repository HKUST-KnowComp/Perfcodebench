#include "interface.h"
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  int64_t total = 0;
  #pragma omp parallel for reduction(+:total)
  for (int iter = 0; iter < iters; ++iter) {
    int64_t iter_total = 0;
    for (std::size_t i = 0; i < xs.size(); ++i) {
      for (std::size_t j = i + 1; j < xs.size(); ++j) {
        iter_total += static_cast<int64_t>(std::abs(xs[i] - xs[j])) +
                      static_cast<int64_t>(std::abs(ys[i] - ys[j]));
      }
    }
    total += iter_total;
  }
  return total / iters;
}