#include "interface.h"

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  if (iters <= 0) return 0;
  const auto* cache_ptr = cache.data();
  const auto* page_ids_ptr = page_ids.data();
  const auto* slot_offsets_ptr = slot_offsets.data();
  const std::size_t n = page_ids.size();
  const std::size_t ps = static_cast<std::size_t>(page_size);
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t offset = static_cast<std::size_t>(page_ids_ptr[i]) * ps + static_cast<std::size_t>(slot_offsets_ptr[i]);
    hash ^= static_cast<uint64_t>(cache_ptr[offset]);
    hash *= 1099511628211ULL;
  }
  return hash;
}
