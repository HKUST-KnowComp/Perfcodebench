#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* l = left.data();
    const uint32_t* l_end = l + left.size();
    const uint32_t* r = right.data();
    const uint32_t* r_end = r + right.size();
    
    uint64_t c = 0;
    uint32_t last = 0;
    bool has_last = false;
    
    while (l < l_end && r < r_end) {
      uint32_t val;
      if (*l <= *r) {
        val = *l++;
      } else {
        val = *r++;
      }
      if (!has_last || val != last) {
        ++c;
        last = val;
        has_last = true;
      }
    }
    
    while (l < l_end) {
      uint32_t val = *l++;
      if (!has_last || val != last) {
        ++c;
        last = val;
        has_last = true;
      }
    }
    
    while (r < r_end) {
      uint32_t val = *r++;
      if (!has_last || val != last) {
        ++c;
        last = val;
        has_last = true;
      }
    }
    
    count = c;
  }
  
  return count;
}