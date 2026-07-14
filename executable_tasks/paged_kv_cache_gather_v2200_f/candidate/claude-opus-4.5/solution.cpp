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
    
    // Process with prefetching for better cache utilization
    constexpr std::size_t PREFETCH_DIST = 16;
    
    for (; i + PREFETCH_DIST < n; ++i) {
      // Prefetch future access
      std::size_t future_idx = static_cast<std::size_t>(pids[i + PREFETCH_DIST]) * ps + 
                               static_cast<std::size_t>(soffs[i + PREFETCH_DIST]);
      __builtin_prefetch(&cache_ptr[future_idx], 0, 0);
      
      // Current access
      std::size_t idx = static_cast<std::size_t>(pids[i]) * ps + static_cast<std::size_t>(soffs[i]);
      uint64_t value = cache_ptr[idx];
      hash ^= value;
      hash *= 1099511628211ULL;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      std::size_t idx = static_cast<std::size_t>(pids[i]) * ps + static_cast<std::size_t>(soffs[i]);
      uint64_t value = cache_ptr[idx];
      hash ^= value;
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}
