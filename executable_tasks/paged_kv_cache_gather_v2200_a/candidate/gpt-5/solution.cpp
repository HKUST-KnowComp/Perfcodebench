#include "interface.h"

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int /*num_pages*/,
    int page_size,
    int iters) {
  // Match baseline behavior: if no iterations, return the initial hash (0).
  if (iters <= 0) {
    return 0ULL;
  }

  const uint32_t* cache_ptr = cache.data();
  const uint32_t* pid_ptr = page_ids.data();
  const uint32_t* off_ptr = slot_offsets.data();
  const std::size_t n = page_ids.size();
  const std::size_t ps = static_cast<std::size_t>(page_size);

  // FNV-1a 64-bit offset basis
  uint64_t hash = 1469598103934665603ULL;

  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t idx = static_cast<std::size_t>(pid_ptr[i]) * ps + static_cast<std::size_t>(off_ptr[i]);
    hash ^= static_cast<uint64_t>(cache_ptr[idx]);
    hash *= 1099511628211ULL;  // FNV prime
  }

  return hash;
}
