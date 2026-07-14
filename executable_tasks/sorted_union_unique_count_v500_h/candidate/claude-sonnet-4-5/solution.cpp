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
  if (left_size == 0) {
    // Count unique in right only
    for (int iter = 0; iter < iters; ++iter) {
      count = 1;
      for (size_t i = 1; i < right_size; ++i) {
        count += (right[i] != right[i-1]);
      }
    }
    return count;
  }
  if (right_size == 0) {
    // Count unique in left only
    for (int iter = 0; iter < iters; ++iter) {
      count = 1;
      for (size_t i = 1; i < left_size; ++i) {
        count += (left[i] != left[i-1]);
      }
    }
    return count;
  }
  
  const uint32_t* left_ptr = left.data();
  const uint32_t* right_ptr = right.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0, j = 0;
    uint32_t last_value;
    count = 0;
    
    // Initialize with the smaller of the first elements
    if (left_ptr[0] <= right_ptr[0]) {
      last_value = left_ptr[0];
      i = 1;
      if (left_ptr[0] == right_ptr[0]) {
        j = 1;
      }
    } else {
      last_value = right_ptr[0];
      j = 1;
    }
    count = 1;
    
    // Two-pointer merge with duplicate elimination
    while (i < left_size && j < right_size) {
      uint32_t left_val = left_ptr[i];
      uint32_t right_val = right_ptr[j];
      
      if (left_val < right_val) {
        if (left_val != last_value) {
          last_value = left_val;
          ++count;
        }
        ++i;
      } else if (right_val < left_val) {
        if (right_val != last_value) {
          last_value = right_val;
          ++count;
        }
        ++j;
      } else {
        // left_val == right_val
        if (left_val != last_value) {
          last_value = left_val;
          ++count;
        }
        ++i;
        ++j;
      }
    }
    
    // Process remaining elements from left
    while (i < left_size) {
      uint32_t val = left_ptr[i];
      if (val != last_value) {
        last_value = val;
        ++count;
      }
      ++i;
    }
    
    // Process remaining elements from right
    while (j < right_size) {
      uint32_t val = right_ptr[j];
      if (val != last_value) {
        last_value = val;
        ++count;
      }
      ++j;
    }
  }
  
  return count;
}