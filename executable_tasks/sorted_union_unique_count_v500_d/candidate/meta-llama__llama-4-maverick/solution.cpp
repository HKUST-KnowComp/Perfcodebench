#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    size_t i = 0, j = 0;
    while (i < left.size() || j < right.size()) {
      uint32_t val = (i < left.size() && (j == right.size() || left[i] <= right[j])) ? left[i++] : right[j++];
      if (count == 0 || val > left[i - 1]) {
        ++count;
      }
    }
  }
  return count;
}
