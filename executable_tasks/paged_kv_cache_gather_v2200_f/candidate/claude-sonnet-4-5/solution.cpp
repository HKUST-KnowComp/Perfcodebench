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
  const std::size_t lookup_size = page_ids.size();
  const uint32_t* cache_ptr = cache.data();
  const uint32_t* page_ids_ptr = page_ids.data();
  const uint32_t* slot_offsets_ptr = slot_offsets.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t i = 0;
    const std::size_t unroll_limit = lookup_size - (lookup_size % 4);
    
    // Unrolled loop for better ILP
    for (; i < unroll_limit; i += 4) {
      const std::size_t idx0 = static_cast<std::size_t>(page_ids_ptr[i]) * static_cast<std::size_t>(page_size) + static_cast<std::size_t>(slot_offsets_ptr[i]);
      const std::size_t idx1 = static_cast<std::size_t>(page_ids_ptr[i+1]) * static_cast<std::size_t>(page_size) + static_cast<std::size_t>(slot_offsets_ptr[i+1]);
      const std::size_t idx2 = static_cast<std::size_t>(page_ids_ptr[i+2]) * static_cast<std::size_t>(page_size) + static_cast<std::size_t>(slot_offsets_ptr[i+2]);
      const std::size_t idx3 = static_cast<std::size_t>(page_ids_ptr[i+3]) * static_cast<std::size_t>(page_size) + static_cast<std::size_t>(slot_offsets_ptr[i+3]);
      
      // Prefetch next iteration
      if (i + 8 < lookup_size) {
        __builtin_prefetch(&cache_ptr[static_cast<std::size_t>(page_ids_ptr[i+4]) * static_cast<std::size_t>(page_size) + static_cast<std::size_t>(slot_offsets_ptr[i+4])], 0, 3);
      }
      
      hash = mix(hash, cache_ptr[idx0]);
      hash = mix(hash, cache_ptr[idx1]);
      hash = mix(hash, cache_ptr[idx2]);
      hash = mix(hash, cache_ptr[idx3]);
    }
    
    // Handle remaining elements
    for (; i < lookup_size; ++i) {
      const std::size_t idx = static_cast<std::size_t>(page_ids_ptr[i]) * static_cast<std::size_t>(page_size) + static_cast<std::size_t>(slot_offsets_ptr[i]);
      hash = mix(hash, cache_ptr[idx]);
    }
  }
  
  return hash;
}