#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  (void)iters; // Unused, result is invariant across iterations
  uint64_t count = 0;
  size_t i = 0, j = 0;
  size_t n = left.size();
  size_t m = right.size();
  uint32_t last = 0;
  bool first = true;
  while (i < n || j < m) {
    uint32_t val;
    if (i < n && (j == m || left[i] <= right[j])) {
      val = left[i];
      ++i;
    } else {
      val = right[j];
      ++j;
    }
    if (first || val != last) {
      ++count;
      last = val;
      first = false;
    }
  }
  return count;
}