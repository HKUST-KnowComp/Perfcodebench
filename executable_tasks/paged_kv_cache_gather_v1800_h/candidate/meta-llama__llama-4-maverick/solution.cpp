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
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < page_ids.size(); ++i) {
      uint32_t page_id = page_ids[i];
      uint32_t slot_offset = slot_offsets[i];
      uint32_t index = page_id * static_cast<uint32_t>(page_size) + slot_offset;
      hash = mix(hash, cache[static_cast<std::size_t>(index)]);
    }
  }
  return hash;
}
