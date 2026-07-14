#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>
#ifdef _OPENMP
#include <omp.h>
#endif

static inline int64_t sum_abs_pairs_dim(const std::vector<int32_t>& a) {
  std::size_t n = a.size();
  if (n < 2) return 0;
  std::vector<int32_t> v(a.begin(), a.end());
  std::sort(v.begin(), v.end());
  int64_t total = 0;
  int64_t prefix = 0;
  for (std::size_t i = 0; i < n; ++i) {
    total += static_cast<int64_t>(v[i]) * static_cast<int64_t>(i) - prefix;
    prefix += static_cast<int64_t>(v[i]);
  }
  return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int /*iters*/) {
  // Result does not depend on iters; compute once.
  if (xs.size() <= 1) return 0;
  int64_t sx = 0, sy = 0;
#ifdef _OPENMP
#pragma omp parallel sections
  {
#pragma omp section
    { sx = sum_abs_pairs_dim(xs); }
#pragma omp section
    { sy = sum_abs_pairs_dim(ys); }
  }
#else
  sx = sum_abs_pairs_dim(xs);
  sy = sum_abs_pairs_dim(ys);
#endif
  return sx + sy;
}
