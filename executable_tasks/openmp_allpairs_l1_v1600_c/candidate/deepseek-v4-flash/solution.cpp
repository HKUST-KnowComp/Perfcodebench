#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  if (iters <= 0) return 0;
  const int64_t n = static_cast<int64_t>(xs.size());
  const int32_t* xs_data = xs.data();
  const int32_t* ys_data = ys.data();
  int64_t total = 0;

#pragma omp parallel for reduction(+:total) schedule(static, 1)
  for (int64_t i = 0; i < n; ++i) {
    for (int64_t j = i + 1; j < n; ++j) {
      total += static_cast<int64_t>(xs_data[i] - xs_data[j] >= 0 ? xs_data[i] - xs_data[j] : xs_data[j] - xs_data[i])
             + static_cast<int64_t>(ys_data[i] - ys_data[j] >= 0 ? ys_data[i] - ys_data[j] : ys_data[j] - ys_data[i]);
    }
  }
  return total;
}
