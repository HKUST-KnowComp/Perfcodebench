#include "interface.h"

#include <cstddef>

static inline uint64_t unique_count_single(const uint32_t* p, std::size_t n) {
  if (n == 0) return 0;
  uint64_t cnt = 0;
  std::size_t i = 0;
  while (i < n) {
    ++cnt;
    uint32_t v = p[i];
    do {
      ++i;
    } while (i < n && p[i] == v);
  }
  return cnt;
}

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int /*iters*/) {
  const std::size_t n = left.size();
  const std::size_t m = right.size();
  const uint32_t* lp = left.data();
  const uint32_t* rp = right.data();

  // Fast paths for empty inputs
  if (n == 0) return unique_count_single(rp, m);
  if (m == 0) return unique_count_single(lp, n);

  std::size_t i = 0, j = 0;
  uint64_t count = 0;

  while (i < n && j < m) {
    uint32_t a = lp[i];
    uint32_t b = rp[j];
    if (a < b) {
      ++count;
      do { ++i; } while (i < n && lp[i] == a);
    } else if (b < a) {
      ++count;
      do { ++j; } while (j < m && rp[j] == b);
    } else { // a == b
      ++count;
      do { ++i; } while (i < n && lp[i] == a);
      do { ++j; } while (j < m && rp[j] == b);
    }
  }

  // Consume any remaining unique runs in either array
  while (i < n) {
    uint32_t a = lp[i];
    ++count;
    do { ++i; } while (i < n && lp[i] == a);
  }
  while (j < m) {
    uint32_t b = rp[j];
    ++count;
    do { ++j; } while (j < m && rp[j] == b);
  }

  return count;
}
