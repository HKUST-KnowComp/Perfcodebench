#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  (void)num_pages;
  
  const std::size_t n = page_ids.size();
  if (n == 0) {
    uint64_t hash = 1469598103934665603ULL;
    return hash;
  }
  
  const uint32_t* __restrict__ cache_ptr = cache.data();
  const uint32_t* __restrict__ pids = page_ids.data();
  const uint32_t* __restrict__ soffs = slot_offsets.data();
  const uint32_t ps = static_cast<uint32_t>(page_size);
  
  // Precompute indices for gather
  std::vector<uint32_t> indices(n);
  uint32_t* idx_ptr = indices.data();
  for (std::size_t i = 0; i < n; ++i) {
    idx_ptr[i] = pids[i] * ps + soffs[i];
  }
  
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  
  uint64_t hash = FNV_OFFSET;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    
    std::size_t i = 0;
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    
    // Unrolled loop - 4 at a time
    for (; i < n4; i += 4) {
      uint64_t v0 = cache_ptr[idx_ptr[i]];
      uint64_t v1 = cache_ptr[idx_ptr[i + 1]];
      uint64_t v2 = cache_ptr[idx_ptr[i + 2]];
      uint64_t v3 = cache_ptr[idx_ptr[i + 3]];
      
      hash ^= v0;
      hash *= FNV_PRIME;
      hash ^= v1;
      hash *= FNV_PRIME;
      hash ^= v2;
      hash *= FNV_PRIME;
      hash ^= v3;
      hash *= FNV_PRIME;
    }
    
    // Handle remainder
    for (; i < n; ++i) {
      hash ^= cache_ptr[idx_ptr[i]];
      hash *= FNV_PRIME;
    }
  }
  
  return hash;
}