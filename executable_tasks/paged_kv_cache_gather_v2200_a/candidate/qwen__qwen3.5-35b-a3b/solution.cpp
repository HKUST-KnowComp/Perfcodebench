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
  std::vector<size_t> indices(page_ids.size());
  for (size_t i = 0; i < page_ids.size(); ++i) {
    indices[i] = static_cast<size_t>(page_ids[i]) * static_cast<size_t>(page_size) + static_cast<size_t>(slot_offsets[i]);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < indices.size(); ++i) {
      hash = mix(hash, cache[indices[i]]);
    }
  }
  return hash;
}