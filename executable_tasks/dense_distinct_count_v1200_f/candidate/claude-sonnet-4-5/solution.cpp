#include "interface.h"

#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  (void)iters;
  
  // Use bitset for O(1) lookup with better cache performance
  const size_t num_bytes = (key_space + 7) / 8;
  std::vector<uint8_t> seen(num_bytes, 0);
  
  // Mark all keys as seen
  for (uint32_t key : keys) {
    seen[key >> 3] |= (1u << (key & 7));
  }
  
  // Count set bits
  uint64_t count = 0;
  for (size_t i = 0; i < num_bytes; ++i) {
    count += __builtin_popcount(seen[i]);
  }
  
  return count;
}