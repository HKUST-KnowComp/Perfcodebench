#include "interface.h"

#include <algorithm>
#include <vector>
#include <cstdint>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  
  uint64_t final_count = 0;
  const size_t n = left.size();
  const size_t m = right.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t count = 0;
    size_t i = 0;
    size_t j = 0;

    while (i < n && j < m) {
      uint32_t val;
      uint32_t l_val = left[i];
      uint32_t r_val = right[j];

      if (l_val < r_val) {
        val = l_val;
        i++;
        while (i < n && left[i] == val) i++;
      } else if (r_val < l_val) {
        val = r_val;
        j++;
        while (j < m && right[j] == val) j++;
      } else {
        val = l_val;
        i++;
        j++;
        while (i < n && left[i] == val) i++;
        while (j < m && right[j] == val) j++;
      }
      count++;
    }

    while (i < n) {
      uint32_t val = left[i++];
      count++;
      while (i < n && left[i] == val) i++;
    }

    while (j < m) {
      uint32_t val = right[j++];
      count++;
      while (j < m && right[j] == val) j++;
    }
    
    final_count = count;
  }

  return final_count;
}