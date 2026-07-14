#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline void sort_int32_inplace(int32_t* a, int n) {
  if (n <= 1) return;

  int32_t* stack_l[64];
  int32_t* stack_r[64];
  int top = 0;
  stack_l[top] = a;
  stack_r[top] = a + (n - 1);
  ++top;

  while (top) {
    --top;
    int32_t* l = stack_l[top];
    int32_t* r = stack_r[top];

    while (l < r) {
      int32_t* i = l;
      int32_t* j = r;
      const int32_t pivot = l[(r - l) >> 1];

      do {
        while (*i < pivot) ++i;
        while (*j > pivot) --j;
        if (i <= j) {
          const int32_t tmp = *i;
          *i = *j;
          *j = tmp;
          ++i;
          --j;
        }
      } while (i <= j);

      if (j - l < r - i) {
        if (i < r) {
          stack_l[top] = i;
          stack_r[top] = r;
          ++top;
        }
        r = j;
      } else {
        if (l < j) {
          stack_l[top] = l;
          stack_r[top] = j;
          ++top;
        }
        l = i;
      }
    }
  }
}

static inline int64_t axis_sum_sorted(const int32_t* a, int n) {
  int64_t prefix = 0;
  int64_t total = 0;
  for (int i = 0; i < n; ++i) {
    const int64_t v = static_cast<int64_t>(a[i]);
    total += v * static_cast<int64_t>(i) - prefix;
    prefix += v;
  }
  return total;
}

}  // namespace

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const int n = static_cast<int>(xs.size());
  if (iters <= 0 || n <= 1) return 0;

  std::vector<int32_t> xbuf(xs.begin(), xs.end());
  std::vector<int32_t> ybuf(ys.begin(), ys.end());

  int64_t xsum = 0;
  int64_t ysum = 0;

#pragma omp parallel sections
  {
#pragma omp section
    {
      sort_int32_inplace(xbuf.data(), n);
      xsum = axis_sum_sorted(xbuf.data(), n);
    }
#pragma omp section
    {
      sort_int32_inplace(ybuf.data(), n);
      ysum = axis_sum_sorted(ybuf.data(), n);
    }
  }

  const int64_t total = xsum + ysum;
  (void)iters;
  return total;
}
