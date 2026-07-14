#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

/**
 * FNV-1a style mix function used for checksum calculation.
 */
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= static_cast<uint64_t>(value);
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
  
  // The baseline implementation was copying the entire cache into a vector of vectors
  // inside the iteration loop. This is extremely slow due to allocations and memory traffic.
  // Since the cache is already a flat vector, we can calculate the index directly:
  // index = page_id * page_size + slot_offset.

  uint64_t hash = 0;
  const size_t num_elements = page_ids.size();
  const uint32_t* cache_ptr = cache.data();
  const uint32_t* p_ids = page_ids.data();
  const uint32_t* s_offsets = slot_offsets.data();
  const uint64_t p_size = static_cast<uint64_t>(page_size);

  for (int iter = 0; iter < iters; ++iter) {
    // Reset hash to the FNV offset basis at the start of each iteration
    hash = 1469598103934665603ULL;
    
    // Gather values from the flat cache using the provided page IDs and offsets
    for (size_t i = 0; i < num_elements; ++i) {
      // Calculate the flat index into the cache
      uint64_t index = static_cast<uint64_t>(p_ids[i]) * p_size + static_cast<uint64_t>(s_offsets[i]);
      hash = mix(hash, cache_ptr[index]);
    }
  }

  return hash;
}