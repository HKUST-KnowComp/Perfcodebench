#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int /*iters*/) {
  const uint32_t* p = left.data();
  const uint32_t* q = right.data();
  const uint32_t* pe = p + left.size();
  const uint32_t* qe = q + right.size();

  uint64_t count = 0;

  while (p < pe && q < qe) {
    uint32_t a = *p;
    uint32_t b = *q;
    if (a == b) {
      ++count;
      do { ++p; } while (p < pe && *p == a);
      do { ++q; } while (q < qe && *q == b);
    } else if (a < b) {
      ++count;
      do { ++p; } while (p < pe && *p == a);
    } else { // a > b
      ++count;
      do { ++q; } while (q < qe && *q == b);
    }
  }

  while (p < pe) {
    ++count;
    uint32_t a = *p;
    do { ++p; } while (p < pe && *p == a);
  }

  while (q < qe) {
    ++count;
    uint32_t b = *q;
    do { ++q; } while (q < qe && *q == b);
  }

  return count;
}
