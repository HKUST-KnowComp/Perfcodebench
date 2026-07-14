#include "interface.h"

#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  const uint32_t* __restrict__ cache_ptr = cache.data();
  const uint32_t* __restrict__ page_ids_ptr = page_ids.data();
  const uint32_t* __restrict__ slot_offsets_ptr = slot_offsets.data();
  const std::size_t num_queries = page_ids.size();
  const std::size_t page_size_sz = static_cast<std::size_t>(page_size);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t i = 0;
    const std::size_t unroll_limit = num_queries - (num_queries % 4);
    
    // Unrolled loop for better ILP
    for (; i < unroll_limit; i += 4) {
      // Prefetch next iteration
      if (i + 8 < num_queries) {
        __builtin_prefetch(&cache_ptr[page_ids_ptr[i + 4] * page_size_sz + slot_offsets_ptr[i + 4]], 0, 0);
        __builtin_prefetch(&cache_ptr[page_ids_ptr[i + 5] * page_size_sz + slot_offsets_ptr[i + 5]], 0, 0);
      }
      
      const std::size_t idx0 = page_ids_ptr[i] * page_size_sz + slot_offsets_ptr[i];
      const std::size_t idx1 = page_ids_ptr[i + 1] * page_size_sz + slot_offsets_ptr[i + 1];
      const std::size_t idx2 = page_ids_ptr[i + 2] * page_size_sz + slot_offsets_ptr[i + 2];
      const std::size_t idx3 = page_ids_ptr[i + 3] * page_size_sz + slot_offsets_ptr[i + 3];
      
      const uint32_t val0 = cache_ptr[idx0];
      const uint32_t val1 = cache_ptr[idx1];
      const uint32_t val2 = cache_ptr[idx2];
      const uint32_t val3 = cache_ptr[idx3];
      
      hash = mix(hash, val0);
      hash = mix(hash, val1);
      hash = mix(hash, val2);
      hash = mix(hash, val3);
    }
    
    // Handle remaining elements
    for (; i < num_queries; ++i) {
      const std::size_t idx = page_ids_ptr[i] * page_size_sz + slot_offsets_ptr[i];
      hash = mix(hash, cache_ptr[idx]);
    }
  }
  
  return hash;
}