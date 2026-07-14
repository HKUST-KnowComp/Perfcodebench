#pragma once

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;
  uint32_t last = 0;
  bool first = true;
  
  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    first = true;
    size_t i = 0, j = 0;
    
    while (i < left.size() && j < right.size()) {
      uint32_t current;
      if (left[i] < right[j]) {
        current = left[i++];
      } else if (right[j] < left[i]) {
        current = right[j++];
      } else {
        current = left[i++];
        j++;
      }
      
      if (first || current != last) {
        count++;
        last = current;
        first = false;
      }
    }
    
    while (i < left.size()) {
      if (first || left[i] != last) {
        count++;
        last = left[i];
        first = false;
      }
      i++;
    }
    
    while (j < right.size()) {
      if (first || right[j] != last) {
        count++;
        last = right[j];
        first = false;
      }
      j++;
    }
  }
  
  return count;
}