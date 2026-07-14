#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  if (iters <= 0) return 0;

  size_t i = 0, j = 0;
  uint64_t distinct_count = 0;
  uint32_t last_val = 0;
  bool first = true;

  while (i < left.size() || j < right.size()) {
    uint32_t current;
    if (i < left.size() && (j == right.size() || left[i] < right[j])) {
      current = left[i++];
    } else if (j < right.size() && (i == left.size() || right[j] < left[i])) {
      current = right[j++];
    } else {
      current = left[i];
      i++;
      j++;
    }

    if (first || current != last_val) {
      distinct_count++;
      last_val = current;
      first = false;
    }
  }

  return distinct_count;
}