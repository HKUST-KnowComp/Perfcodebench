#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline void merge_sort_int32(int32_t* data, int32_t* buf, std::size_t n) {
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
        if (data[i] <= data[j]) {
          buf[k++] = data[i++];
        } else {
          buf[k++] = data[j++];
        }
      }
      while (i < mid) buf[k++] = data[i++];
      while (j < right) buf[k++] = data[j++];
    }

    for (std::size_t i = 0; i < n; ++i) data[i] = buf[i];
  }
}

static inline int64_t sorted_pairwise_l1_1d(const int32_t* a, std::size_t n) {
  int64_t sum = 0;
  int64_t prefix = 0;
  for (std::size_t i = 0; i < n; ++i) {
    const int64_t v = static_cast<int64_t>(a[i]);
    sum += v * static_cast<int64_t>(i) - prefix;
    prefix += v;
  }
  return sum;
}

}  // namespace

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const std::size_t n = xs.size();
  if (n <= 1 || iters <= 0) return 0;

  std::vector<int32_t> sx(xs.begin(), xs.end());
  std::vector<int32_t> sy(ys.begin(), ys.end());
  std::vector<int32_t> bx(n);
  std::vector<int32_t> by(n);

  #pragma omp parallel sections
  {
    #pragma omp section
    {
      merge_sort_int32(sx.data(), bx.data(), n);
    }
    #pragma omp section
    {
      merge_sort_int32(sy.data(), by.data(), n);
    }
  }

  int64_t xsum = 0;
  int64_t ysum = 0;

  #pragma omp parallel sections
  {
    #pragma omp section
    {
      xsum = sorted_pairwise_l1_1d(sx.data(), n);
    }
    #pragma omp section
    {
      ysum = sorted_pairwise_l1_1d(sy.data(), n);
    }
  }

  const int64_t total = xsum + ysum;
  (void)iters;
  return total;
}
