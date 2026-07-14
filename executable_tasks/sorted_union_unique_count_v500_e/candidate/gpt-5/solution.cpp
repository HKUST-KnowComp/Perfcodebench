#include "interface.h"

#include <cstdint>
#include <vector>

static inline uint64_t count_unique_single(const uint32_t* arr, size_t n) {
  if (n == 0) return 0;
  uint64_t c = 0;
  size_t i = 0;
  while (i < n) {
    uint32_t v = arr[i];
    ++c;
    do { ++i; } while (i < n && arr[i] == v);
  }
  return c;
}

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  const uint32_t* a = left.data();
  const uint32_t* b = right.data();
  const size_t n = left.size();
  const size_t m = right.size();

  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    if (n == 0) {
      count = count_unique_single(b, m);
      continue;
    }
    if (m == 0) {
      count = count_unique_single(a, n);
      continue;
    }

    size_t i = 0, j = 0;
    uint64_t c = 0;

    while (i < n && j < m) {
      uint32_t av = a[i];
      uint32_t bv = b[j];
      if (av < bv) {
        ++c;
        do { ++i; } while (i < n && a[i] == av);
      } else if (bv < av) {
        ++c;
        do { ++j; } while (j < m && b[j] == bv);
      } else { // av == bv
        ++c;
        do { ++i; } while (i < n && a[i] == av);
        do { ++j; } while (j < m && b[j] == bv);
      }
    }

    while (i < n) {
      uint32_t av = a[i];
      ++c;
      do { ++i; } while (i < n && a[i] == av);
    }
    while (j < m) {
      uint32_t bv = b[j];
      ++c;
      do { ++j; } while (j < m && b[j] == bv);
    }

    count = c;
  }

  return count;
}
