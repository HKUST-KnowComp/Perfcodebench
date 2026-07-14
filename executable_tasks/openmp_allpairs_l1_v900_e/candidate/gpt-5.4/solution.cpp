#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline void swap_i32(int32_t& a, int32_t& b) {
  int32_t t = a;
  a = b;
  b = t;
}

static void insertion_sort(std::vector<int32_t>& v) {
  const std::size_t n = v.size();
  for (std::size_t i = 1; i < n; ++i) {
    int32_t key = v[i];
    std::size_t j = i;
    while (j > 0 && v[j - 1] > key) {
      v[j] = v[j - 1];
      --j;
    }
    v[j] = key;
  }
}

static void quicksort_impl(int32_t* a, int left, int right) {
  while (left < right) {
    int i = left;
    int j = right;
    const int32_t pivot = a[left + ((right - left) >> 1)];

    while (i <= j) {
      while (a[i] < pivot) ++i;
      while (a[j] > pivot) --j;
      if (i <= j) {
        swap_i32(a[i], a[j]);
        ++i;
        --j;
      }
    }

    if (j - left < right - i) {
      if (left < j) quicksort_impl(a, left, j);
      left = i;
    } else {
      if (i < right) quicksort_impl(a, i, right);
      right = j;
    }
  }
}

static void sort_i32(std::vector<int32_t>& v) {
  const std::size_t n = v.size();
  if (n <= 1) return;
  if (n < 32) {
    insertion_sort(v);
    return;
  }
  quicksort_impl(v.data(), 0, static_cast<int>(n - 1));
}

static int64_t pairwise_abs_sum_1d(const std::vector<int32_t>& vals) {
  const std::size_t n = vals.size();
  if (n < 2) return 0;

  std::vector<int32_t> sorted(vals);
  sort_i32(sorted);

  int64_t prefix = 0;
  int64_t total = 0;
  for (std::size_t i = 0; i < n; ++i) {
    const int64_t x = static_cast<int64_t>(sorted[i]);
    total += x * static_cast<int64_t>(i) - prefix;
    prefix += x;
  }
  return total;
}

}  // namespace

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  (void)iters;
  return pairwise_abs_sum_1d(xs) + pairwise_abs_sum_1d(ys);
}
