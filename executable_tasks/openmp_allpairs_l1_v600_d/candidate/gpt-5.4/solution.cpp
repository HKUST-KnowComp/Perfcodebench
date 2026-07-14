#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline void merge_sort_int32(int32_t* a, int32_t* tmp, std::size_t n) {
  if (n <= 1) return;

  for (std::size_t width = 1; width < n; width <<= 1) {
    for (std::size_t left = 0; left < n; left += (width << 1)) {
      std::size_t mid = left + width;
      if (mid > n) mid = n;
      std::size_t right = left + (width << 1);
      if (right > n) right = n;

      std::size_t i = left;
      std::size_t j = mid;
      std::size_t k = left;

      while (i < mid && j < right) {
        if (a[i] <= a[j]) {
          tmp[k++] = a[i++];
        } else {
          tmp[k++] = a[j++];
        }
      }
      while (i < mid) tmp[k++] = a[i++];
      while (j < right) tmp[k++] = a[j++];

      for (std::size_t t = left; t < right; ++t) a[t] = tmp[t];
    }
  }
}

static inline int64_t sorted_pairwise_abs_sum(const int32_t* a, std::size_t n) {
  int64_t prefix = 0;
  int64_t total = 0;
  for (std::size_t i = 0; i < n; ++i) {
    const int64_t v = static_cast<int64_t>(a[i]);
    total += v * static_cast<int64_t>(i) - prefix;
    prefix += v;
  }
  return total;
}

}  // namespace

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const std::size_t n = xs.size();
  if (n < 2 || iters <= 0) return 0;

  std::vector<int32_t> xsorted(xs.begin(), xs.end());
  std::vector<int32_t> ysorted(ys.begin(), ys.end());
  std::vector<int32_t> xtmp(n);
  std::vector<int32_t> ytmp(n);

  #pragma omp parallel sections
  {
    #pragma omp section
    {
      merge_sort_int32(xsorted.data(), xtmp.data(), n);
    }
    #pragma omp section
    {
      merge_sort_int32(ysorted.data(), ytmp.data(), n);
    }
  }

  int64_t xsum = 0;
  int64_t ysum = 0;

  #pragma omp parallel sections
  {
    #pragma omp section
    {
      xsum = sorted_pairwise_abs_sum(xsorted.data(), n);
    }
    #pragma omp section
    {
      ysum = sorted_pairwise_abs_sum(ysorted.data(), n);
    }
  }

  const int64_t total = xsum + ysum;
  return total;
}
