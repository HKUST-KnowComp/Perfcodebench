#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0, j = 0;
    uint64_t distinct = 0;
    uint32_t last = 0;
    bool first = true;
    while (i < left.size() || j < right.size()) {
      uint32_t val;
      if (j >= right.size() || (i < left.size() && left[i] <= right[j])) {
        val = left[i++];
      } else {
        val = right[j++];
      }
      if (first || val != last) {
        ++distinct;
        last = val;
        first = false;
      }
    }
    count = distinct;
  }
  return count;
}
