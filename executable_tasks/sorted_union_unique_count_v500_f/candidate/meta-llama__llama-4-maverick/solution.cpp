#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    size_t i = 0, j = 0;
    uint32_t last = 0;
    while (i < left.size() || j < right.size()) {
      uint32_t current;
      if (i < left.size() && (j == right.size() || left[i] < right[j])) {
        current = left[i++];
      } else if (j < right.size() && (i == left.size() || right[j] < left[i])) {
        current = right[j++];
      } else {
        current = left[i++];
        j++;
      }
      count += (current != last);
      last = current;
    }
  }
  return count;
}