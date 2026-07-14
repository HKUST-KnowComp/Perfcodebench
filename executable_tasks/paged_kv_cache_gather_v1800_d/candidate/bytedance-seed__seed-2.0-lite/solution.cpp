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
  const size_t page_size_sz = static_cast<size_t>(page_size);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < page_ids.size(); ++i) {
      const size_t page_idx = static_cast<size_t>(page_ids[i]);
      const size_t slot_idx = static_cast<size_t>(slot_offsets[i]);
      const uint32_t val = cache[page_idx * page_size_sz + slot_idx];
      hash = mix(hash, val);
    }
  }
  return hash;
}