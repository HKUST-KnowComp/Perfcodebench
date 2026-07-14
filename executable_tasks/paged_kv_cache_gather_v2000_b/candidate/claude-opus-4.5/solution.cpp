#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  (void)num_pages;
  
  const uint32_t* __restrict__ cache_ptr = cache.data();
  const uint32_t* __restrict__ pids = page_ids.data();
  const uint32_t* __restrict__ soffs = slot_offsets.data();
  const std::size_t n = page_ids.size();
  const std::size_t ps = static_cast<std::size_t>(page_size);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t i = 0;
    
    // Process 4 elements at a time for better ILP
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    for (; i < n4; i += 4) {
      // Prefetch ahead
      if (i + 16 < n) {
        __builtin_prefetch(&cache_ptr[pids[i + 16] * ps + soffs[i + 16]], 0, 0);
      }
      
      uint64_t v0 = cache_ptr[pids[i] * ps + soffs[i]];
      uint64_t v1 = cache_ptr[pids[i + 1] * ps + soffs[i + 1]];
      uint64_t v2 = cache_ptr[pids[i + 2] * ps + soffs[i + 2]];
      uint64_t v3 = cache_ptr[pids[i + 3] * ps + soffs[i + 3]];
      
      hash ^= v0;
      hash *= 1099511628211ULL;
      hash ^= v1;
      hash *= 1099511628211ULL;
      hash ^= v2;
      hash *= 1099511628211ULL;
      hash ^= v3;
      hash *= 1099511628211ULL;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint64_t val = cache_ptr[pids[i] * ps + soffs[i]];
      hash ^= val;
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}