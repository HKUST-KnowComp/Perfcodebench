#include "interface.h"

#include <cstdint>
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
  const uint32_t* offsets_ptr = slot_offsets.data();
  const uint64_t n = static_cast<uint64_t>(page_ids.size());
  const uint64_t ps = static_cast<uint64_t>(page_size);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (uint64_t i = 0; i < n; ++i) {
      uint64_t idx = static_cast<uint64_t>(page_ids_ptr[i]) * ps + static_cast<uint64_t>(offsets_ptr[i]);
      h = mix(h, cache_ptr[idx]);
    }
    hash = h;
  }
  return hash;
}
