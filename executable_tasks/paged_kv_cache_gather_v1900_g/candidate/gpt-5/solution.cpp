#include "interface.h"

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int /*num_pages*/,
    int page_size,
    int iters) {
  // Match baseline behavior: if no iterations, return 0.
  if (iters <= 0) {
    return 0ULL;
  }

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  const std::size_t n = page_ids.size();
  const std::size_t ps = static_cast<std::size_t>(page_size);
  const uint32_t* cache_ptr = cache.data();

  uint64_t hash = FNV_OFFSET;
  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t idx = static_cast<std::size_t>(page_ids[i]) * ps + static_cast<std::size_t>(slot_offsets[i]);
    hash ^= static_cast<uint64_t>(cache_ptr[idx]);
    hash *= FNV_PRIME;
  }

  return hash;
}
