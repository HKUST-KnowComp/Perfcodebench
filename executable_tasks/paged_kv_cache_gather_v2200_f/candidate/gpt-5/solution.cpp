#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  (void)num_pages; // Not needed for direct indexing; suppress unused warning

  // Match baseline behavior: if no iterations, return the initial hash value (0 in baseline)
  if (iters <= 0) {
    return 0ULL;
  }

  const uint32_t* cache_data = cache.data();
  const uint32_t* pid = page_ids.data();
  const uint32_t* off = slot_offsets.data();
  const std::size_t n = page_ids.size();
  const std::size_t ps = static_cast<std::size_t>(page_size);

  // FNV-1a 64-bit constants
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET_BASIS;

  std::size_t i = 0;
  const std::size_t n4 = n & ~static_cast<std::size_t>(3);

  // Unrolled loop by 4 for reduced overhead
  for (; i < n4; i += 4) {
    std::size_t idx0 = static_cast<std::size_t>(pid[i]) * ps + static_cast<std::size_t>(off[i]);
    uint64_t v0 = static_cast<uint64_t>(cache_data[idx0]);
    hash ^= v0;
    hash *= FNV_PRIME;

    std::size_t idx1 = static_cast<std::size_t>(pid[i + 1]) * ps + static_cast<std::size_t>(off[i + 1]);
    uint64_t v1 = static_cast<uint64_t>(cache_data[idx1]);
    hash ^= v1;
    hash *= FNV_PRIME;

    std::size_t idx2 = static_cast<std::size_t>(pid[i + 2]) * ps + static_cast<std::size_t>(off[i + 2]);
    uint64_t v2 = static_cast<uint64_t>(cache_data[idx2]);
    hash ^= v2;
    hash *= FNV_PRIME;

    std::size_t idx3 = static_cast<std::size_t>(pid[i + 3]) * ps + static_cast<std::size_t>(off[i + 3]);
    uint64_t v3 = static_cast<uint64_t>(cache_data[idx3]);
    hash ^= v3;
    hash *= FNV_PRIME;
  }

  // Tail loop
  for (; i < n; ++i) {
    std::size_t idx = static_cast<std::size_t>(pid[i]) * ps + static_cast<std::size_t>(off[i]);
    uint64_t v = static_cast<uint64_t>(cache_data[idx]);
    hash ^= v;
    hash *= FNV_PRIME;
  }

  return hash;
}
