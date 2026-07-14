#include "interface.h"

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
} // namespace

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  const size_t n = page_ids.size();
  const size_t page_size_s = static_cast<size_t>(page_size);
  const uint32_t* cache_data = cache.data();
  const uint32_t* page_ids_data = page_ids.data();
  const uint32_t* slot_offsets_data = slot_offsets.data();
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      size_t idx = static_cast<size_t>(page_ids_data[i]) * page_size_s + static_cast<size_t>(slot_offsets_data[i]);
      hash ^= cache_data[idx];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}