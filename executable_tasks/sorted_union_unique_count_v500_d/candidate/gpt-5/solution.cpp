#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int /*iters*/) {
  // Compute the count of distinct values in the union of two sorted arrays.
  const uint32_t* p = left.data();
  const uint32_t* pend = p + left.size();
  const uint32_t* q = right.data();
  const uint32_t* qend = q + right.size();

  uint64_t count = 0;

  while (p < pend && q < qend) {
    uint32_t a = *p;
    uint32_t b = *q;
    if (a < b) {
      ++count;
      // Skip duplicates of 'a' in left
      do { ++p; } while (p < pend && *p == a);
    } else if (b < a) {
      ++count;
      // Skip duplicates of 'b' in right
      do { ++q; } while (q < qend && *q == b);
    } else { // a == b
      ++count;
      // Skip duplicates of this value in both arrays
      do { ++p; } while (p < pend && *p == a);
      do { ++q; } while (q < qend && *q == b);
    }
  }

  // Remaining elements in left
  while (p < pend) {
    uint32_t a = *p;
    ++count;
    do { ++p; } while (p < pend && *p == a);
  }

  // Remaining elements in right
  while (q < qend) {
    uint32_t b = *q;
    ++count;
    do { ++q; } while (q < qend && *q == b);
  }

  return count;
}
