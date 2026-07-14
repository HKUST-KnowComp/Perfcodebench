#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    size_t i = 0, j = 0;
    uint32_t prev = 0;
    bool has_prev = false;
    while (i < left.size() || j < right.size()) {
      uint32_t val;
      if (j == right.size() || (i < left.size() && left[i] < right[j])) {
        val = left[i++];
      } else if (i == left.size() || left[i] > right[j]) {
        val = right[j++];
      } else {
        val = left[i++];
        j++;
      }
      if (!has_prev || val != prev) {
        count++;
        prev = val;
        has_prev = true;
      }
    }
  }
  return count;
}