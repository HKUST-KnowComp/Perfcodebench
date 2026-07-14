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
      std::size_t right = left + (width << 1);
      if (mid > n) mid = n;
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

static inline int64_t sorted_axis_pair_sum(const int32_t* a, std::size_t n) {
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

  std::vector<int32_t> xcopy(xs.begin(), xs.end());
  std::vector<int32_t> ycopy(ys.begin(), ys.end());
  std::vector<int32_t> xbuf(n);
  std::vector<int32_t> ybuf(n);

#pragma omp parallel sections
  {
#pragma omp section
    { merge_sort_int32(xcopy.data(), xbuf.data(), n); }
#pragma omp section
    { merge_sort_int32(ycopy.data(), ybuf.data(), n); }
  }

  int64_t xsum = 0;
  int64_t ysum = 0;
#pragma omp parallel sections
  {
#pragma omp section
    { xsum = sorted_axis_pair_sum(xcopy.data(), n); }
#pragma omp section
    { ysum = sorted_axis_pair_sum(ycopy.data(), n); }
  }

  const int64_t total = xsum + ysum;
  return total;
}
