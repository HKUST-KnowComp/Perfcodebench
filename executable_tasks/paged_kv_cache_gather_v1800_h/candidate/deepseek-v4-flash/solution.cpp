#include "interface.h"

#include <vector>

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  const uint32_t* cache_data = cache.data();
  const uint32_t* page_ids_data = page_ids.data();
  const uint32_t* offsets_data = slot_offsets.data();
  std::size_t n = page_ids.size();
  std::size_t ps = static_cast<std::size_t>(page_size);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      uint64_t val = cache_data[
          static_cast<std::size_t>(page_ids_data[i]) * ps +
          static_cast<std::size_t>(offsets_data[i])];
      hash ^= val;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}