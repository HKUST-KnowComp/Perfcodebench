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
  if (n == 0 || iters <= 0) {
    return 1469598103934665603ULL;
  }
  
  // Precompute flat indices once
  std::vector<std::size_t> indices(n);
  const std::size_t ps = static_cast<std::size_t>(page_size);
  for (std::size_t i = 0; i < n; ++i) {
    indices[i] = static_cast<std::size_t>(page_ids[i]) * ps + static_cast<std::size_t>(slot_offsets[i]);
  }
  
  const uint32_t* __restrict__ cache_ptr = cache.data();
  const std::size_t* __restrict__ idx_ptr = indices.data();
  
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  
  uint64_t hash = FNV_OFFSET;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    
    std::size_t i = 0;
    
    // Process 4 elements at a time for better ILP
    const std::size_t n4 = n & ~std::size_t(3);
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
    
    // Handle remaining elements
    for (; i < n; ++i) {
      hash ^= cache_ptr[idx_ptr[i]];
      hash *= FNV_PRIME;
    }
  }
  
  return hash;
}
