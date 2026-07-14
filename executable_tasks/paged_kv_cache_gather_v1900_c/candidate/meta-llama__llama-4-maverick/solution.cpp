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
  uint64_t hash = 0;
  const std::size_t cache_size = cache.size();
  const std::size_t num_elements = page_ids.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < num_elements; ++i) {
      const std::size_t page_id = static_cast<std::size_t>(page_ids[i]);
      const std::size_t slot_offset = static_cast<std::size_t>(slot_offsets[i]);
      const std::size_t index = page_id * static_cast<std::size_t>(page_size) + slot_offset;
      if (index < cache_size) {
        hash = mix(hash, cache[index]);
      }
    }
  }
  return hash;
}
