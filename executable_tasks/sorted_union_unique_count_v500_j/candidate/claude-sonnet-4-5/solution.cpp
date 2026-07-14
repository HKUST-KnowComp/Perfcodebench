#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  const size_t left_size = left.size();
  const size_t right_size = right.size();
  
  // Handle edge cases outside the loop
  if (left_size == 0 && right_size == 0) return 0;
  if (left_size == 0) {
    // Count unique in right only
    uint64_t count = 0;
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
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
      count = 1;
      for (size_t i = 1; i < left_size; ++i) {
        count += (left[i] != left[i-1]);
      }
    }
    return count;
  }
  
  uint64_t count = 0;
  const uint32_t* left_ptr = left.data();
  const uint32_t* right_ptr = right.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0, j = 0;
    count = 0;
    uint32_t last_value = 0;
    bool has_value = false;
    
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
        current = left_ptr[i];
        ++i;
        ++j;
      }
      
      if (!has_value || current != last_value) {
        ++count;
        last_value = current;
        has_value = true;
      }
    }
    
    // Process remaining elements from left
    while (i < left_size) {
      uint32_t current = left_ptr[i];
      if (!has_value || current != last_value) {
        ++count;
        last_value = current;
        has_value = true;
      }
      ++i;
    }
    
    // Process remaining elements from right
    while (j < right_size) {
      uint32_t current = right_ptr[j];
      if (!has_value || current != last_value) {
        ++count;
        last_value = current;
        has_value = true;
      }
      ++j;
    }
  }
  
  return count;
}