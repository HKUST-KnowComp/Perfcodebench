#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* l_ptr = left.data();
    const uint32_t* r_ptr = right.data();
    const uint32_t* l_end = l_ptr + left.size();
    const uint32_t* r_end = r_ptr + right.size();
    
    count = 0;
    uint32_t last = 0;
    bool has_last = false;
    
    while (l_ptr < l_end && r_ptr < r_end) {
      uint32_t val;
      if (*l_ptr < *r_ptr) {
        val = *l_ptr++;
      } else if (*r_ptr < *l_ptr) {
        val = *r_ptr++;
      } else {
        val = *l_ptr;
        ++l_ptr;
        ++r_ptr;
      }
      
      if (!has_last || val != last) {
        ++count;
        last = val;
        has_last = true;
      }
    }
    
    while (l_ptr < l_end) {
      uint32_t val = *l_ptr++;
      if (!has_last || val != last) {
        ++count;
        last = val;
        has_last = true;
      }
    }
    
    while (r_ptr < r_end) {
      uint32_t val = *r_ptr++;
      if (!has_last || val != last) {
        ++count;
        last = val;
        has_last = true;
      }
    }
  }
  
  return count;
}