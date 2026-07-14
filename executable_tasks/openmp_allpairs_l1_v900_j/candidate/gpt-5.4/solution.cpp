#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

static inline void sort_int32_inplace(int32_t* a, int n) {
  if (n <= 1) return;

  int32_t stack_l[64];
  int32_t stack_r[64];
  int top = 0;
  stack_l[top] = 0;
  stack_r[top] = n - 1;
  ++top;

  while (top) {
    --top;
    int l = stack_l[top];
    int r = stack_r[top];

    while (l < r) {
      int i = l;
      int j = r;
      int32_t pivot = a[l + ((r - l) >> 1)];

      while (i <= j) {
        while (a[i] < pivot) ++i;
        while (a[j] > pivot) --j;
        if (i <= j) {
          int32_t tmp = a[i];
          a[i] = a[j];
          a[j] = tmp;
          ++i;
          --j;
        }
      }

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

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const int n = static_cast<int>(xs.size());
  if (n <= 1 || iters <= 0) return 0;

  int64_t result = 0;

  #pragma omp parallel
  {
    std::vector<int32_t> xbuf;
    std::vector<int32_t> ybuf;
    int64_t local_result = 0;

    #pragma omp single nowait
    {
      xbuf.assign(xs.begin(), xs.end());
      ybuf.assign(ys.begin(), ys.end());
      sort_int32_inplace(xbuf.data(), n);
      sort_int32_inplace(ybuf.data(), n);
      local_result = axis_sum_sorted(xbuf.data(), n) + axis_sum_sorted(ybuf.data(), n);
      result = local_result;
    }
  }

  return result;
}
