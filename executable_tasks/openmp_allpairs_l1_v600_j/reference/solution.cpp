#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <omp.h>
#include <vector>

namespace {

int num_threads() {
  return std::max(1, std::min(8, omp_get_max_threads()));
}

}  // namespace

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  omp_set_num_threads(num_threads());
  int64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    total = 0;
#pragma omp parallel for reduction(+ : total) schedule(static)
    for (std::size_t i = 0; i < xs.size(); ++i) {
      int64_t local = 0;
      for (std::size_t j = i + 1; j < xs.size(); ++j) {
        local += static_cast<int64_t>(std::abs(xs[i] - xs[j])) +
                 static_cast<int64_t>(std::abs(ys[i] - ys[j]));
      }
      total += local;
    }
  }
  return total;
}
