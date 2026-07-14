#include "interface.h"

#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0, j = 0;
    const size_t left_size = left.size();
    const size_t right_size = right.size();
    uint64_t distinct_count = 0;
    uint32_t last_value = 0;
    bool has_last = false;
    
    while (i < left_size && j < right_size) {
      uint32_t current;
      if (left[i] < right[j]) {
        current = left[i++];
      } else if (left[i] > right[j]) {
        current = right[j++];
      } else {
        current = left[i];
        ++i;
        ++j;
      }
      
      if (!has_last || current != last_value) {
        ++distinct_count;
        last_value = current;
        has_last = true;
      }
    }
    
    while (i < left_size) {
      if (!has_last || left[i] != last_value) {
        ++distinct_count;
        last_value = left[i];
        has_last = true;
      }
      ++i;
    }
    
    while (j < right_size) {
      if (!has_last || right[j] != last_value) {
        ++distinct_count;
        last_value = right[j];
        has_last = true;
      }
      ++j;
    }
    
    count = distinct_count;
  }
  
  return count;
}