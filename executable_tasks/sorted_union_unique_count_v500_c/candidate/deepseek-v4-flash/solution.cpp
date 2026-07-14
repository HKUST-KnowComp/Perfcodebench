#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t count = 0;
  size_t i = 0, j = 0;
  bool first = true;
  uint32_t last = 0;

  while (i < left.size() && j < right.size()) {
    uint32_t val;
    if (left[i] < right[j]) {
      val = left[i++];
    } else if (left[i] > right[j]) {
      val = right[j++];
    } else {
      val = left[i++];
      j++;
    }
    if (first || val != last) {
      ++count;
      last = val;
      first = false;
    }
  }

  while (i < left.size()) {
    uint32_t val = left[i++];
    if (first || val != last) {
      ++count;
      last = val;
      first = false;
    }
  }

  while (j < right.size()) {
    uint32_t val = right[j++];
    if (first || val != last) {
      ++count;
      last = val;
      first = false;
    }
  }

  return count;
}