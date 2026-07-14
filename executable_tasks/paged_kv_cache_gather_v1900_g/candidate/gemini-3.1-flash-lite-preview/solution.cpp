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
  const size_t num_lookups = page_ids.size();
  const uint32_t* cache_ptr = cache.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < num_lookups; ++i) {
      // Calculate flat index: page_id * page_size + offset
      size_t flat_idx = static_cast<size_t>(page_ids[i]) * static_cast<size_t>(page_size) + 
                        static_cast<size_t>(slot_offsets[i]);
      hash = mix(hash, static_cast<uint64_t>(cache_ptr[flat_idx]));
    }
  }
  return hash;
}