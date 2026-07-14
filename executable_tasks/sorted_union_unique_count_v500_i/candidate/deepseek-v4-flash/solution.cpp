#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0, j = 0;
    size_t n = left.size(), m = right.size();
    uint32_t last = 0;
    bool first = true;
    count = 0;
    while (i < n && j < m) {
      if (left[i] < right[j]) {
        if (first || left[i] != last) {
          ++count;
          last = left[i];
          first = false;
        }
        ++i;
      } else if (right[j] < left[i]) {
        if (first || right[j] != last) {
          ++count;
          last = right[j];
          first = false;
        }
        ++j;
      } else {
        if (first || left[i] != last) {
          ++count;
          last = left[i];
          first = false;
        }
        ++i; ++j;
      }
    }
    while (i < n) {
      if (first || left[i] != last) {
        ++count;
        last = left[i];
        first = false;
      }
      ++i;
    }
    while (j < m) {
      if (first || right[j] != last) {
        ++count;
        last = right[j];
        first = false;
      }
      ++j;
    }
  }
  return count;
}
