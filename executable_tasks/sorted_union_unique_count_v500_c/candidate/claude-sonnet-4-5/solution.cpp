#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;
  
  const size_t left_size = left.size();
  const size_t right_size = right.size();
  
  // Early exit for empty cases
  if (left_size == 0 && right_size == 0) {
    return 0;
  }
  
  const uint32_t* left_ptr = left.data();
  const uint32_t* right_ptr = right.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0, j = 0;
    uint64_t distinct_count = 0;
    uint32_t last_value = 0;
    bool has_last = false;
    
    // Two-pointer merge with deduplication
    while (i < left_size && j < right_size) {
      uint32_t current;
      if (left_ptr[i] < right_ptr[j]) {
        current = left_ptr[i];
        ++i;
      } else if (left_ptr[i] > right_ptr[j]) {
        current = right_ptr[j];
        ++j;
      } else {
        // Equal values - take one and advance both
        current = left_ptr[i];
        ++i;
        ++j;
      }
      
      if (!has_last || current != last_value) {
        ++distinct_count;
        last_value = current;
        has_last = true;
      }
    }
    
    // Process remaining elements from left
    while (i < left_size) {
      uint32_t current = left_ptr[i];
      if (!has_last || current != last_value) {
        ++distinct_count;
        last_value = current;
        has_last = true;
      }
      ++i;
    }
    
    // Process remaining elements from right
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