#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline void merge_sort_int32(int32_t* data, int32_t* buf, std::size_t n) {
  if (n <= 1) return;

  int32_t* src = data;
  int32_t* dst = buf;

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
        if (src[i] <= src[j]) {
          dst[k++] = src[i++];
        } else {
          dst[k++] = src[j++];
        }
      }
      while (i < mid) dst[k++] = src[i++];
      while (j < right) dst[k++] = src[j++];
    }
    int32_t* tmp = src;
    src = dst;
    dst = tmp;
  }

  if (src != data) {
    for (std::size_t i = 0; i < n; ++i) data[i] = src[i];
  }
}

static inline int64_t sorted_pairwise_l1_1d(const std::vector<int32_t>& v) {
  const std::size_t n = v.size();
  if (n < 2) return 0;

  std::vector<int32_t> work(v.begin(), v.end());
  std::vector<int32_t> buf(n);
  merge_sort_int32(work.data(), buf.data(), n);

  int64_t prefix = 0;
  int64_t total = 0;
  for (std::size_t i = 0; i < n; ++i) {
    const int64_t x = static_cast<int64_t>(work[i]);
    total += x * static_cast<int64_t>(i) - prefix;
    prefix += x;
  }
  return total;
}

}  // namespace

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  if (iters <= 0) return 0;
  const std::size_t n = xs.size();
  if (n < 2) return 0;

  int64_t xsum = 0;
  int64_t ysum = 0;

#pragma omp parallel sections
  {
#pragma omp section
    { xsum = sorted_pairwise_l1_1d(xs); }
#pragma omp section
    { ysum = sorted_pairwise_l1_1d(ys); }
  }

  const int64_t total = xsum + ysum;
  return total;
}
