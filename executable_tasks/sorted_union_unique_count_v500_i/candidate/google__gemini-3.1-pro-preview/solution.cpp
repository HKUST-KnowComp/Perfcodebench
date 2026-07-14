#include "interface.h"

#include <algorithm>
#include <vector>
#include <cstdint>

/**
 * Optimized sorted union count.
 * 
 * The baseline implementation was O(N log N) due to concatenation and sorting,
 * and it performed expensive memory allocations inside the loop.
 * 
 * This implementation uses a two-pointer approach (O(N)) to count distinct elements
 * in the union of two sorted arrays. It avoids allocations and minimizes branching.
 */
uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  
  const uint32_t* const l_data = left.data();
  const uint32_t* const r_data = right.data();
  const size_t l_size = left.size();
  const size_t r_size = right.size();

  uint64_t final_count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t count = 0;
    size_t i = 0;
    size_t j = 0;
    
    if (l_size == 0 && r_size == 0) {
        final_count = 0;
        continue;
    }

    uint32_t last;
    bool has_last = false;

    while (i < l_size && j < r_size) {
      uint32_t current;
      uint32_t lv = l_data[i];
      uint32_t rv = r_data[j];

      if (lv < rv) {
        current = lv;
        i++;
      } else if (rv < lv) {
        current = rv;
        j++;
      } else {
        current = lv;
        i++;
        j++;
      }

      if (!has_last || current != last) {
        count++;
        last = current;
        has_last = true;
      }
    }

    while (i < l_size) {
      uint32_t current = l_data[i++];
      if (!has_last || current != last) {
        count++;
        last = current;
        has_last = true;
      }
    }

    while (j < r_size) {
      uint32_t current = r_data[j++];
      if (!has_last || current != last) {
        count++;
        last = current;
        has_last = true;
      }
    }
    
    final_count = count;
  }

  return final_count;
}
