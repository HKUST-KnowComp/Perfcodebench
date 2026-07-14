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
    while (i < left.size() || j < right.size()) {
      uint32_t curr;
      if (i == left.size()) {
        curr = right[j++];
      } else if (j == right.size()) {
        curr = left[i++];
      } else if (left[i] < right[j]) {
        curr = left[i++];
      } else {
        curr = right[j++];
      }
      count += (curr != prev);
      prev = curr;
    }
  }
  return count;
}