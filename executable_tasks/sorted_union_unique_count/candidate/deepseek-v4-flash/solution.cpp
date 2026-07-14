#include "interface.h"

#include <cstddef>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  // Compute the distinct count once using a two-pointer merge.
  // This is the same for every iteration because inputs are constant.
  // The loop is retained to satisfy the harness timing expectation;
  // the work inside is now O(N) per iteration.
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset per iteration (cheap: only local variables).
    size_t i = 0, j = 0;
    const size_t n = left.size(), m = right.size();
    uint64_t c = 0;
    uint32_t last = 0;
    bool first = true;

    while (i < n && j < m) {
      uint32_t a = left[i];
      uint32_t b = right[j];
      if (a < b) {
        if (first || a != last) {
          last = a;
          first = false;
          ++c;
        }
        ++i;
      } else if (b < a) {
        if (first || b != last) {
          last = b;
          first = false;
          ++c;
        }
        ++j;
      } else { // equal
        if (first || a != last) {
          last = a;
          first = false;
          ++c;
        }
        ++i;
        ++j;
      }
    }

    // Process remaining elements in left
    while (i < n) {
      uint32_t a = left[i];
      if (first || a != last) {
        last = a;
        first = false;
        ++c;
      }
      ++i;
    }

    // Process remaining elements in right
    while (j < m) {
      uint32_t b = right[j];
      if (first || b != last) {
        last = b;
        first = false;
        ++c;
      }
      ++j;
    }

    count = c;  // same every iteration
  }
  return count;
}
