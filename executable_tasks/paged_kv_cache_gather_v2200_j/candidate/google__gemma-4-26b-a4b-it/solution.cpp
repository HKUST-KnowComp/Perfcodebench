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
  
  uint64_t hash = 0;
  const size_t num_gather_ops = page_ids.size();
  const uint32_t* cache_ptr = cache.data();
  const uint32_t* p_ids = page_ids.data();
  const uint32_t* s_offs = slot_offsets.data();
  const size_t ps = static_cast<size_t>(page_size);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < num_gather_ops; ++i) {
      // Direct indexing into the flat cache array avoids the O(N*M) copy overhead
      uint32_t val = cache_ptr[static_cast<size_t>(p_ids[i]) * ps + s_offs[i]];
      hash = mix(hash, static_cast<uint64_t>(val));
    }
  }
  
  return hash;
}