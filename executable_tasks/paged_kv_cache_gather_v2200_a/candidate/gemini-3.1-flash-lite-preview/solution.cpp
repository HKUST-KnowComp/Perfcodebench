#include "interface.h"

#include <vector>
#include <cstdint>

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
  
  uint64_t hash = 0;
  const uint32_t* cache_ptr = cache.data();
  const uint32_t* p_ids = page_ids.data();
  const uint32_t* s_offsets = slot_offsets.data();
  size_t num_lookups = page_ids.size();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < num_lookups; ++i) {
      // Direct access to cache: page_id * page_size + offset
      uint32_t val = cache_ptr[static_cast<size_t>(p_ids[i]) * static_cast<size_t>(page_size) + static_cast<size_t>(s_offsets[i])];
      hash = mix(hash, static_cast<uint64_t>(val));
    }
  }
  return hash;
}