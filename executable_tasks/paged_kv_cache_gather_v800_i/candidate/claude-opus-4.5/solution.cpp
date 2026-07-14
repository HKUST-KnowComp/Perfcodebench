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
  const uint32_t ps = static_cast<uint32_t>(page_size);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t i = 0;
    
    // Process 4 elements at a time to reduce loop overhead
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    for (; i < n4; i += 4) {
      uint32_t idx0 = pids[i] * ps + soffs[i];
      uint32_t idx1 = pids[i+1] * ps + soffs[i+1];
      uint32_t idx2 = pids[i+2] * ps + soffs[i+2];
      uint32_t idx3 = pids[i+3] * ps + soffs[i+3];
      
      uint64_t v0 = cache_ptr[idx0];
      uint64_t v1 = cache_ptr[idx1];
      uint64_t v2 = cache_ptr[idx2];
      uint64_t v3 = cache_ptr[idx3];
      
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
      uint32_t idx = pids[i] * ps + soffs[i];
      hash ^= cache_ptr[idx];
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}
