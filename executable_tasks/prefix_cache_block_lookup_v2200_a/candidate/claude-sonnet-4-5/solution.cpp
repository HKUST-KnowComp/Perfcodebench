#include "interface.h"

#include <algorithm>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Galloping search: exponential search followed by binary search
// Optimized for sequential lookups with temporal locality
template<typename Iterator, typename T>
bool galloping_search(Iterator begin, Iterator end, const T& value, Iterator& hint) {
  if (begin == end) return false;
  
  // Start from hint position
  Iterator base = hint;
  if (base >= end) base = begin;
  
  // Check if we're already at or past the value
  if (base != end && *base >= value) {
    if (*base == value) {
      hint = base;
      return true;
    }
    // Value might be before hint, fall back to binary search from beginning
    base = begin;
  }
  
  // Galloping phase: exponentially increase step size
  Iterator probe = base;
  std::size_t step = 1;
  std::size_t remaining = end - base;
  
  while (step < remaining) {
    probe = base + step;
    if (*probe >= value) {
      break;
    }
    step <<= 1;
  }
  
  // Binary search in the bounded range
  Iterator left = (step >> 1) < remaining ? base + (step >> 1) : base;
  Iterator right = step < remaining ? base + step + 1 : end;
  
  auto it = std::lower_bound(left, right, value);
  hint = it;
  
  return it != end && *it == value;
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
  const uint32_t* blocks = request_blocks.data();
  const int* offsets = request_offsets.data();
  const std::size_t num_requests = request_offsets.size() - 1;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const uint32_t* hint = cache_begin;
    
    for (std::size_t r = 0; r < num_requests; ++r) {
      const int start = offsets[r];
      const int end = offsets[r + 1];
      uint64_t prefix = 0;
      
      for (int i = start; i < end; ++i) {
        const uint32_t block = blocks[i];
        
        if (!galloping_search(cache_begin, cache_end, block, hint)) {
          break;
        }
        ++prefix;
      }
      
      hash = mix(hash, prefix);
    }
  }
  
  return hash;
}
