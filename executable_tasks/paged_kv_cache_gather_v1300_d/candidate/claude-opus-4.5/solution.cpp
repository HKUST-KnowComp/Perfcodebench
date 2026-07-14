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
  
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    
    std::size_t i = 0;
    
    // Process 4 elements at a time with prefetching
    const std::size_t n4 = (n / 4) * 4;
    for (; i < n4; i += 4) {
      // Prefetch ahead
      if (i + 16 < n) {
        __builtin_prefetch(&cache_ptr[static_cast<std::size_t>(pids[i + 16]) * ps + soffs[i + 16]], 0, 0);
        __builtin_prefetch(&cache_ptr[static_cast<std::size_t>(pids[i + 17]) * ps + soffs[i + 17]], 0, 0);
        __builtin_prefetch(&cache_ptr[static_cast<std::size_t>(pids[i + 18]) * ps + soffs[i + 18]], 0, 0);
        __builtin_prefetch(&cache_ptr[static_cast<std::size_t>(pids[i + 19]) * ps + soffs[i + 19]], 0, 0);
      }
      
      uint64_t v0 = cache_ptr[static_cast<std::size_t>(pids[i]) * ps + soffs[i]];
      uint64_t v1 = cache_ptr[static_cast<std::size_t>(pids[i + 1]) * ps + soffs[i + 1]];
      uint64_t v2 = cache_ptr[static_cast<std::size_t>(pids[i + 2]) * ps + soffs[i + 2]];
      uint64_t v3 = cache_ptr[static_cast<std::size_t>(pids[i + 3]) * ps + soffs[i + 3]];
      
      hash ^= v0;
      hash *= FNV_PRIME;
      hash ^= v1;
      hash *= FNV_PRIME;
      hash ^= v2;
      hash *= FNV_PRIME;
      hash ^= v3;
      hash *= FNV_PRIME;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint64_t val = cache_ptr[static_cast<std::size_t>(pids[i]) * ps + soffs[i]];
      hash ^= val;
      hash *= FNV_PRIME;
    }
  }
  
  return hash;
}