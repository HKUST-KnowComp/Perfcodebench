#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t count_unique_single_sorted(const std::vector<uint32_t>& v) {
  const size_t n = v.size();
  if (n == 0) return 0;

  uint64_t count = 1;
  uint32_t prev = v[0];
  for (size_t i = 1; i < n; ++i) {
    const uint32_t x = v[i];
    if (x != prev) {
      ++count;
      prev = x;
    }
  }
  return count;
}

static inline uint64_t count_union_sorted(const std::vector<uint32_t>& left,
                                          const std::vector<uint32_t>& right) {
  const size_t n = left.size();
  const size_t m = right.size();

  if (n == 0) return count_unique_single_sorted(right);
  if (m == 0) return count_unique_single_sorted(left);

  // Fast path when value ranges do not overlap.
  if (left[n - 1] < right[0]) {
    return count_unique_single_sorted(left) + count_unique_single_sorted(right);
  }
  if (right[m - 1] < left[0]) {
    return count_unique_single_sorted(left) + count_unique_single_sorted(right);
  }

  size_t i = 0;
  size_t j = 0;
  uint64_t count = 0;
  uint32_t last = 0;
  bool have_last = false;

  while (i < n && j < m) {
    uint32_t v;
    const uint32_t a = left[i];
    const uint32_t b = right[j];

    if (a < b) {
      v = a;
      do {
        ++i;
      } while (i < n && left[i] == v);
    } else if (b < a) {
      v = b;
      do {
        ++j;
      } while (j < m && right[j] == v);
    } else {
      v = a;
      do {
        ++i;
      } while (i < n && left[i] == v);
      do {
        ++j;
      } while (j < m && right[j] == v);
    }

    if (!have_last || v != last) {
      ++count;
      last = v;
      have_last = true;
    }
  }

  while (i < n) {
    const uint32_t v = left[i];
    do {
      ++i;
    } while (i < n && left[i] == v);
    if (!have_last || v != last) {
      ++count;
      last = v;
      have_last = true;
    }
  }

  while (j < m) {
    const uint32_t v = right[j];
    do {
      ++j;
    } while (j < m && right[j] == v);
    if (!have_last || v != last) {
      ++count;
      last = v;
      have_last = true;
    }
  }

  return count;
}

}  // namespace

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    count = count_union_sorted(left, right);
  }
  return count;
}
