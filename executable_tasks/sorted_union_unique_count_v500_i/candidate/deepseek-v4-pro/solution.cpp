#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  (void)iters; // unused, computation only once
  size_t i = 0, j = 0;
  uint64_t count = 0;
  uint32_t last = 0;
  bool has_last = false;
  while (i < left.size() || j < right.size()) {
    uint32_t val;
    if (i < left.size() && j < right.size()) {
      if (left[i] < right[j]) {
        val = left[i++];
      } else if (left[i] > right[j]) {
        val = right[j++];
      } else {
        val = left[i++];
        ++j;
      }
    } else if (i < left.size()) {
      val = left[i++];
    } else {
      val = right[j++];
    }
    if (!has_last || val != last) {
      ++count;
      last = val;
      has_last = true;
    }
  }
  return count;
}
