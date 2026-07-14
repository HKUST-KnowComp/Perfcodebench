#include "interface.h"

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t hash = 1469598103934665603ULL;
  const size_t page_size_u = static_cast<size_t>(page_size);
  const size_t n = page_ids.size();
  for (size_t i = 0; i < n; ++i) {
    size_t idx = static_cast<size_t>(page_ids[i]) * page_size_u + static_cast<size_t>(slot_offsets[i]);
    uint32_t val = cache[idx];
    hash = mix(hash, val);
  }
  return hash;
}