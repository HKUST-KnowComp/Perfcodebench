#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  if (left.empty() && right.empty()) {
    return 0;
  }

  uint64_t count = 0;
  size_t i = 0;
  size_t j = 0;
  uint32_t last_val = 0;
  bool first = true;

  // We only need to compute the result once as the inputs are constant across iters.
  // However, to strictly follow the loop structure if it were to affect logic:
  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    i = 0;
    j = 0;
    first = true;

    while (i < left.size() || j < right.size()) {
      uint32_t current;
      if (i < left.size() && j < right.size()) {
        if (left[i] < right[j]) {
          current = left[i++];
        } else if (left[i] > right[j]) {
          current = right[j++];
        } else {
          current = left[i];
          i++;
          j++;
        }
      } else if (i < left.size()) {
        current = left[i++];
      } else {
        current = right[j++];
      }

      if (first || current != last_val) {
        count++;
        last_val = current;
        first = false;
      }
    }
  }

  return count;
}