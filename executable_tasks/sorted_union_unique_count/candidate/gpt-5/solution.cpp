#include "interface.h"

#include <cstdint>
#include <vector>

static inline uint64_t count_unique_sorted(const std::vector<uint32_t>& v) {
  if (v.empty()) return 0;
  uint64_t cnt = 1;
  const size_t n = v.size();
  for (size_t i = 1; i < n; ++i) {
    if (v[i] != v[i - 1]) ++cnt;
  }
  return cnt;
}

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int /*iters*/) {
  // If one side is empty, just count distinct values in the other.
  if (left.empty()) return count_unique_sorted(right);
  if (right.empty()) return count_unique_sorted(left);

  const uint32_t* p = left.data();
  const uint32_t* q = right.data();
  const uint32_t* pend = p + left.size();
  const uint32_t* qend = q + right.size();

  uint64_t count = 0;

  while (p < pend && q < qend) {
    uint32_t a = *p;
    uint32_t b = *q;
    if (a < b) {
      ++count;
      do { ++p; } while (p < pend && *p == a);
    } else if (b < a) {
      ++count;
      do { ++q; } while (q < qend && *q == b);
    } else { // a == b
      ++count;
      do { ++p; } while (p < pend && *p == a);
      do { ++q; } while (q < qend && *q == b);
    }
  }

  // Count remaining unique values in left
  while (p < pend) {
    uint32_t a = *p;
    ++count;
    do { ++p; } while (p < pend && *p == a);
  }
  // Count remaining unique values in right
  while (q < qend) {
    uint32_t b = *q;
    ++count;
    do { ++q; } while (q < qend && *q == b);
  }

  return count;
}
