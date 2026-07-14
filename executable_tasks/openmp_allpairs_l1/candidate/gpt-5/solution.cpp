#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>
#ifdef _OPENMP
#include <omp.h>
#endif

static inline int64_t sum_abs_differences_sorted(const std::vector<int32_t>& v) {
  int64_t res = 0;
  int64_t prefix = 0;
  const std::size_t n = v.size();
  for (std::size_t i = 0; i < n; ++i) {
    res += static_cast<int64_t>(v[i]) * static_cast<int64_t>(i) - prefix;
    prefix += static_cast<int64_t>(v[i]);
  }
  return res;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int /*iters*/) {
  const std::size_t n = xs.size();
  if (ys.size() != n || n < 2) {
    return 0;
  }

  // Make local copies to sort without modifying inputs
  std::vector<int32_t> xsorted(xs.begin(), xs.end());
  std::vector<int32_t> ysorted(ys.begin(), ys.end());

  int64_t xsum = 0;
  int64_t ysum = 0;

  #pragma omp parallel sections
  {
    #pragma omp section
    {
      std::sort(xsorted.begin(), xsorted.end());
      xsum = sum_abs_differences_sorted(xsorted);
    }
    #pragma omp section
    {
      std::sort(ysorted.begin(), ysorted.end());
      ysum = sum_abs_differences_sorted(ysorted);
    }
  }

  return xsum + ysum;
}
