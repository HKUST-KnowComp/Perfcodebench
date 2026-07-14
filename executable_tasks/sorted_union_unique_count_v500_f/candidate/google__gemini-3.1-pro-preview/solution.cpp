#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  
  const size_t n_left = left.size();
  const size_t n_right = right.size();
  const uint32_t* const p_left = left.data();
  const uint32_t* const p_right = right.data();

  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    size_t i = 0;
    size_t j = 0;
    
    uint32_t last_val = 0;
    bool has_last = false;

    while (i < n_left && j < n_right) {
      uint32_t current;
      uint32_t l_val = p_left[i];
      uint32_t r_val = p_right[j];

      if (l_val < r_val) {
        current = l_val;
        i++;
      } else if (r_val < l_val) {
        current = r_val;
        j++;
      } else {
        current = l_val;
        i++;
        j++;
      }

      if (!has_last || current != last_val) {
        count++;
        last_val = current;
        has_last = true;
      }
    }

    while (i < n_left) {
      uint32_t current = p_left[i++];
      if (!has_last || current != last_val) {
        count++;
        last_val = current;
        has_last = true;
      }
    }

    while (j < n_right) {
      uint32_t current = p_right[j++];
      if (!has_last || current != last_val) {
        count++;
        last_val = current;
        has_last = true;
      }
    }
  }

  return count;
}
