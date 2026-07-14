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
  
  const uint32_t* cache_ptr = cache.data();
  const uint32_t* page_ids_ptr = page_ids.data();
  const uint32_t* slot_offsets_ptr = slot_offsets.data();
  const size_t gather_size = page_ids.size();
  const size_t p_size = static_cast<size_t>(page_size);
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < gather_size; ++i) {
      // Direct access to the cache instead of copying to a temporary 2D structure
      uint32_t val = cache_ptr[static_cast<size_t>(page_ids_ptr[i]) * p_size + slot_offsets_ptr[i]];
      hash = mix(hash, val);
    }
  }
  return hash;
}