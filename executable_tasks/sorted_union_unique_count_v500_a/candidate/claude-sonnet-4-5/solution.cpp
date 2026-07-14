#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  if (iters == 0) return 0;
  
  uint64_t count = 0;
  size_t i = 0, j = 0;
  uint32_t last = 0;
  bool has_last = false;
  
  while (i < left.size() && j < right.size()) {
    uint32_t val;
    if (left[i] < right[j]) {
      val = left[i++];
    } else if (left[i] > right[j]) {
      val = right[j++];
    } else {
      val = left[i++];
      j++;
    }
    
    if (!has_last || val != last) {
      count++;
      last = val;
      has_last = true;
    }
  }
  
  while (i < left.size()) {
    if (!has_last || left[i] != last) {
      count++;
      last = left[i];
      has_last = true;
    }
    i++;
  }
  
  while (j < right.size()) {
    if (!has_last || right[j] != last) {
      count++;
      last = right[j];
      has_last = true;
    }
    j++;
  }
  
  return count;
}