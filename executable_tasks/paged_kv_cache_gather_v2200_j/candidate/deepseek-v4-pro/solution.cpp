#include "interface.h"

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  const size_t gather_count = page_ids.size();
  const size_t page_size_s = static_cast<size_t>(page_size);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < gather_count; ++i) {
      const uint32_t pid = page_ids[i];
      const uint32_t off = slot_offsets[i];
      const uint32_t val = cache[static_cast<size_t>(pid) * page_size_s + static_cast<size_t>(off)];
      hash ^= val;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
