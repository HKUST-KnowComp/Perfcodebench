#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

static inline int64_t sum_abs_pairwise_1d(const std::vector<int32_t>& a) {
  const std::size_t n = a.size();
  if (n <= 1) return 0;
  std::vector<int32_t> v(a);
  std::sort(v.begin(), v.end());
  int64_t s = 0;
  int64_t prefix = 0;
  for (std::size_t i = 0; i < n; ++i) {
    s += static_cast<int64_t>(v[i]) * static_cast<int64_t>(i) - prefix;
    prefix += static_cast<int64_t>(v[i]);
  }
  return s;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int /*iters*/) {
  // Compute once; result is independent of the number of iterations
  int64_t sumx = sum_abs_pairwise_1d(xs);
  int64_t sumy = sum_abs_pairwise_1d(ys);
  return sumx + sumy;
}
