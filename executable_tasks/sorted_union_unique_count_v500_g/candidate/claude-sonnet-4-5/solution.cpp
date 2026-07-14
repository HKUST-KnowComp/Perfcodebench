#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  const size_t left_size = left.size();
  const size_t right_size = right.size();
  
  if (left_size == 0 && right_size == 0) {
    return 0;
  }
  
  const uint32_t* left_ptr = left.data();
  const uint32_t* right_ptr = right.data();
  
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0;
    size_t j = 0;
    uint64_t distinct_count = 0;
    uint32_t last_value = 0;
    bool has_last = false;
    
    while (i < left_size && j < right_size) {
      uint32_t left_val = left_ptr[i];
      uint32_t right_val = right_ptr[j];
      uint32_t current;
      
      if (left_val < right_val) {
        current = left_val;
        ++i;
      } else if (right_val < left_val) {
        current = right_val;
        ++j;
      } else {
        current = left_val;
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
      uint32_t current = left_ptr[i];
      if (!has_last || current != last_value) {
        ++distinct_count;
        last_value = current;
        has_last = true;
      }
      ++i;
    }
    
    while (j < right_size) {
      uint32_t current = right_ptr[j];
      if (!has_last || current != last_value) {
        ++distinct_count;
        last_value = current;
        has_last = true;
      }
      ++j;
    }
    
    count = distinct_count;
  }
  
  return count;
}