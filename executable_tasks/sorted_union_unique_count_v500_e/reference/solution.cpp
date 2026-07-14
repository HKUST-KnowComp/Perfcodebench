#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::size_t i = 0;
    std::size_t j = 0;
    count = 0;
    bool has_last = false;
    uint32_t last = 0;
    while (i < left.size() || j < right.size()) {
      uint32_t value = 0;
      if (j == right.size() || (i < left.size() && left[i] <= right[j])) {
        value = left[i++];
      } else {
        value = right[j++];
      }
      if (!has_last || value != last) {
        last = value;
        has_last = true;
        ++count;
      }
    }
  }
  return count;
}
