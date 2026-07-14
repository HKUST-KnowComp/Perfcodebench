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

  // The problem asks for the count of distinct values in the union.
  // Since inputs are sorted, we use a two-pointer approach similar to merge sort.
  // We only need to perform the logic once if the inputs don't change, 
  // but the 'iters' parameter implies we must respect the loop structure 
  // if the harness expects the work to be repeated (though the result is constant).
  
  for (int iter = 0; iter < iters; ++iter) {
    i = 0;
    j = 0;
    count = 0;
    first = true;

    while (i < left.size() || j < right.size()) {
      uint32_t current;
      if (i < left.size() && j < right.size()) {
        if (left[i] < right[j]) {
          current = left[i++];
        } else if (left[i] > right[j]) {
          current = right[j++];
        } else {
          current = left[i++];
          j++; // Skip duplicate in right
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