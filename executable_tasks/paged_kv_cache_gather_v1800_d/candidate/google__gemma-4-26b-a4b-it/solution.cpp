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
  
  const size_t num_lookups = page_ids.size();
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  
  // Pre-calculate indices to avoid repeated arithmetic in the hot loop
  // and to ensure we only do the work once.
  std::vector<size_t> target_indices(num_lookups);
  for (size_t i = 0; i < num_lookups; ++i) {
    target_indices[i] = static_cast<size_t>(page_ids[i]) * static_cast<size_t>(page_size) + static_cast<size_t>(slot_offsets[i]);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < num_lookups; ++i) {
      hash = mix(hash, static_cast<uint64_t>(cache[target_indices[i]]));
    }
  }
  
  return hash;
}