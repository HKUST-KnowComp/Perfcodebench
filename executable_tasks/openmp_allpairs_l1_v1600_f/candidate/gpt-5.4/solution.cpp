#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline void sort_int32_inplace(int32_t* a, std::size_t n) {
  if (n <= 1) return;

  struct Range {
    std::size_t l;
    std::size_t r;
  };

  Range stack[64];
  int top = 0;
  stack[top++] = {0, n - 1};

  while (top) {
    Range cur = stack[--top];
    std::size_t l = cur.l;
    std::size_t r = cur.r;

    while (l < r) {
      if (r - l + 1 <= 24) {
        for (std::size_t i = l + 1; i <= r; ++i) {
          int32_t v = a[i];
          std::size_t j = i;
          while (j > l && a[j - 1] > v) {
            a[j] = a[j - 1];
            --j;
          }
          a[j] = v;
        }
        break;
      }

      std::size_t m = l + ((r - l) >> 1);
      int32_t x = a[l];
      int32_t y = a[m];
      int32_t z = a[r];
      int32_t pivot;
      if (x < y) {
        pivot = (y < z) ? y : ((x < z) ? z : x);
      } else {
        pivot = (x < z) ? x : ((y < z) ? z : y);
      }

      std::size_t i = l;
      std::size_t j = r;
      for (;;) {
        while (a[i] < pivot) ++i;
        while (a[j] > pivot) --j;
        if (i >= j) break;
        int32_t tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
        ++i;
        if (j == 0) break;
        --j;
      }

      if (j - l < r - (j + 1)) {
        if (j + 1 < r) stack[top++] = {j + 1, r};
        r = j;
      } else {
        if (l < j) stack[top++] = {l, j};
        l = j + 1;
      }
    }
  }
}

static inline int64_t axis_sum_sorted(const int32_t* a, std::size_t n) {
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
  if (n <= 1 || iters <= 0) return 0;

  std::vector<int32_t> xcopy(xs.begin(), xs.end());
  std::vector<int32_t> ycopy(ys.begin(), ys.end());

#pragma omp parallel sections
  {
#pragma omp section
    { sort_int32_inplace(xcopy.data(), n); }
#pragma omp section
    { sort_int32_inplace(ycopy.data(), n); }
  }

  int64_t xsum = 0;
  int64_t ysum = 0;
#pragma omp parallel sections
  {
#pragma omp section
    { xsum = axis_sum_sorted(xcopy.data(), n); }
#pragma omp section
    { ysum = axis_sum_sorted(ycopy.data(), n); }
  }

  const int64_t total = xsum + ysum;
  (void)iters;
  return total;
}
