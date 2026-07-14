#include "interface.h"

#include <algorithm>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Galloping search: start from hint position and exponentially search forward
inline const uint32_t* galloping_search(
    const uint32_t* begin,
    const uint32_t* end,
    uint32_t value,
    const uint32_t* hint) {
  
  // Start from hint position
  const uint32_t* pos = hint;
  
  if (pos >= end || *pos > value) {
    // Need to search backwards or value not found
    pos = std::lower_bound(begin, end, value);
    return (pos != end && *pos == value) ? pos : end;
  }
  
  if (*pos == value) {
    return pos;
  }
  
  // Gallop forward with exponentially increasing steps
  const uint32_t* base = pos;
  std::size_t step = 1;
  
  while (pos + step < end && *(pos + step) < value) {
    pos += step;
    step <<= 1;
  }
  
  // Binary search in the identified range
  const uint32_t* search_end = (pos + step < end) ? pos + step + 1 : end;
  pos = std::lower_bound(pos, search_end, value);
  
  return (pos != end && *pos == value) ? pos : end;
}

}  // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  (void)hash_space;
  
  const uint32_t* cache_begin = sorted_cache_keys.data();
  const uint32_t* cache_end = cache_begin + sorted_cache_keys.size();
  const uint32_t* blocks_data = request_blocks.data();
  const int* offsets_data = request_offsets.data();
  const std::size_t num_requests = request_offsets.size() - 1;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (std::size_t r = 0; r < num_requests; ++r) {
      const int start = offsets_data[r];
      const int end = offsets_data[r + 1];
      
      uint64_t prefix = 0;
      const uint32_t* hint = cache_begin;
      
      for (int i = start; i < end; ++i) {
        const uint32_t block = blocks_data[i];
        hint = galloping_search(cache_begin, cache_end, block, hint);
        
        if (__builtin_expect(hint == cache_end, 0)) {
          break;
        }
        
        ++prefix;
        ++hint; // Next search likely to be nearby
      }
      
      hash = mix(hash, prefix);
    }
  }
  
  return hash;
}