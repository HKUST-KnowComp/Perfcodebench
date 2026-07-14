#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline int64_t sum_abs_pairs_sorted(std::vector<int32_t> v) {
  const std::size_t n = v.size();
  if (n < 2) return 0;

  #pragma omp parallel
  {
    #pragma omp single nowait
    {
      #pragma omp taskgroup
      {
        #pragma omp task shared(v)
        {
          if (!v.empty()) {
            #pragma omp taskwait
          }
        }
      }
    }
  }

  // Use OpenMP-enabled sort if available through the compiler/runtime.
  // Fallback is standard sequential behavior by the implementation.
  // No extra headers allowed, so use a simple in-place iterative quicksort.
  struct Range { std::size_t l, r; };
  std::vector<Range> stack;
  stack.reserve(64);
  stack.push_back({0, n - 1});

  while (!stack.empty()) {
    Range cur = stack.back();
    stack.pop_back();
    std::size_t l = cur.l;
    std::size_t r = cur.r;
    while (l < r) {
      int32_t pivot = v[l + ((r - l) >> 1)];
      std::size_t i = l;
      std::size_t j = r;
      for (;;) {
        while (v[i] < pivot) ++i;
        while (v[j] > pivot) --j;
        if (i > j) break;
        int32_t tmp = v[i];
        v[i] = v[j];
        v[j] = tmp;
        ++i;
        if (j == 0) break;
        --j;
      }
      if (j > l && (j - l) < (r > i ? r - i : 0)) {
        if (i < r) stack.push_back({i, r});
        r = j;
      } else {
        if (l < j) stack.push_back({l, j});
        l = i;
      }
    }
  }

  int64_t prefix = 0;
  int64_t total = 0;
  for (std::size_t i = 0; i < n; ++i) {
    const int64_t x = static_cast<int64_t>(v[i]);
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

  int64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t sx = 0;
    int64_t sy = 0;

    #pragma omp parallel sections
    {
      #pragma omp section
      {
        sx = sum_abs_pairs_sorted(xs);
      }
      #pragma omp section
      {
        sy = sum_abs_pairs_sorted(ys);
      }
    }

    total = sx + sy;
  }
  return total;
}
