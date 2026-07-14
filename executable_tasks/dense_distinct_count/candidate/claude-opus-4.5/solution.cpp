#include "interface.h"

#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  std::vector<uint8_t> seen(key_space, 0);
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(seen.data(), 0, key_space);
    
    for (uint32_t key : keys) {
      seen[key] = 1;
    }
    
    count = 0;
    for (uint32_t i = 0; i < key_space; ++i) {
      count += seen[i];
    }
  }
  
  return count;
}