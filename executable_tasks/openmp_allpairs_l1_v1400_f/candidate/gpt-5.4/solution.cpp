#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline int64_t sum_abs_pairs_sorted(std::vector<int32_t> v) {
  const std::size_t n = v.size();
  if (n < 2) return 0;

  // In-place heapsort to avoid extra dependencies.
  auto sift_down = [&](std::size_t start, std::size_t end) {
    std::size_t root = start;
    while (true) {
      std::size_t child = root * 2 + 1;
      if (child >= end) break;
      std::size_t swap_idx = root;
      if (v[swap_idx] < v[child]) swap_idx = child;
      if (child + 1 < end && v[swap_idx] < v[child + 1]) swap_idx = child + 1;
      if (swap_idx == root) break;
      int32_t tmp = v[root];
      v[root] = v[swap_idx];
      v[swap_idx] = tmp;
      root = swap_idx;
    }
  };

  for (std::size_t start = n / 2; start > 0; --start) {
    sift_down(start - 1, n);
  }
  for (std::size_t end = n; end > 1; --end) {
    int32_t tmp = v[0];
    v[0] = v[end - 1];
    v[end - 1] = tmp;
    sift_down(0, end - 1);
  }

  int64_t total = 0;
  int64_t prefix = 0;
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
  if (xs.size() != ys.size()) return 0;
  if (xs.size() < 2) return 0;

  int64_t result = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t sx = 0;
    int64_t sy = 0;

#pragma omp parallel sections
    {
#pragma omp section
      { sx = sum_abs_pairs_sorted(xs); }
#pragma omp section
      { sy = sum_abs_pairs_sorted(ys); }
    }

    result = sx + sy;
  }
  return result;
}
