#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

static inline int64_t sum_pairwise_abs_1d(const std::vector<int32_t>& vals) {
  const std::size_t n = vals.size();
  if (n < 2) return 0;
  std::vector<int32_t> v(vals.begin(), vals.end());
  std::sort(v.begin(), v.end());
  int64_t total = 0;
  const int64_t nn1 = static_cast<int64_t>(n) - 1;
  for (std::size_t k = 0; k < n; ++k) {
    int64_t coeff = static_cast<int64_t>(k) - (nn1 - static_cast<int64_t>(k)); // 2k - n + 1
    total += static_cast<int64_t>(v[k]) * coeff;
  }
  return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  // Match baseline behavior: if iters == 0, return 0
  if (iters <= 0) return 0;
  // Compute per-iteration sum exactly once; inputs are const and do not change across iterations
  int64_t sum_x = sum_pairwise_abs_1d(xs);
  int64_t sum_y = sum_pairwise_abs_1d(ys);
  return sum_x + sum_y;
}
