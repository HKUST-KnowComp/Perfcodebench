#include "interface.h"
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  uint64_t hash = 0;
  const size_t num_gather = page_ids.size();
  const size_t u_page_size = static_cast<size_t>(page_size);
  
  // Pre-calculate indices to avoid repeated arithmetic in the hot loop
  // and to ensure we only do the heavy lifting once.
  std::vector<size_t> target_indices(num_gather);
  for (size_t i = 0; i < num_gather; ++i) {
    target_indices[i] = static_cast<size_t>(page_ids[i]) * u_page_size + static_cast<size_t>(slot_offsets[i]);
  }

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < num_gather; ++i) {
      hash = mix(hash, static_cast<uint64_t>(cache[target_indices[i]]));
    }
  }
  
  return hash;
}