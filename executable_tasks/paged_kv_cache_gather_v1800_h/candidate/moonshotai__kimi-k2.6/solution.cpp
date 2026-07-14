#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  (void)num_pages;
  const std::size_t n = page_ids.size();
  const std::size_t p_size = static_cast<std::size_t>(page_size);
  const uint32_t* cache_data = cache.data();
  const uint32_t* page_ids_data = page_ids.data();
  const uint32_t* slot_offsets_data = slot_offsets.data();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      const std::size_t idx =
          static_cast<std::size_t>(page_ids_data[i]) * p_size +
          static_cast<std::size_t>(slot_offsets_data[i]);
      hash = mix(hash, static_cast<uint64_t>(cache_data[idx]));
    }
  }
  return hash;
}
