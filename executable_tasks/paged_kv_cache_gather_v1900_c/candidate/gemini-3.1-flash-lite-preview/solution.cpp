#include "interface.h"

#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  uint64_t hash = 1469598103934665603ULL;
  const size_t num_lookups = page_ids.size();
  
  // Precompute indices to avoid repeated multiplication inside the loop
  std::vector<size_t> indices(num_lookups);
  for (size_t i = 0; i < num_lookups; ++i) {
    indices[i] = static_cast<size_t>(page_ids[i]) * static_cast<size_t>(page_size) + static_cast<size_t>(slot_offsets[i]);
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    for (size_t i = 0; i < num_lookups; ++i) {
      current_hash = mix(current_hash, cache[indices[i]]);
    }
    hash = current_hash;
  }
  return hash;
}