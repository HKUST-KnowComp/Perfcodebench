#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

static inline int64_t sum_pairwise_abs_sorted(const std::vector<int32_t>& v) {
  const std::size_t n = v.size();
  int64_t sum = 0;
  int64_t coeff = -(int64_t)(n > 0 ? (n - 1) : 0);
  for (std::size_t i = 0; i < n; ++i) {
    sum += static_cast<int64_t>(v[i]) * coeff;
    coeff += 2;
  }
  return sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int /*iters*/) {
  const std::size_t n = xs.size();
  if (n == 0) return 0;
  // Assume xs.size() == ys.size() as per task contract

  // Work on sorted copies (cannot modify inputs)
  std::vector<int32_t> sx(xs);
  std::vector<int32_t> sy(ys);
  std::sort(sx.begin(), sx.end());
  std::sort(sy.begin(), sy.end());

  int64_t total = sum_pairwise_abs_sorted(sx) + sum_pairwise_abs_sorted(sy);
  return total;
}
