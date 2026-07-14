#include "interface.h"
#include <cstddef>

namespace {

inline uint64_t mix(uint64_t hash, uint32_t value) {
  hash ^= static_cast<uint64_t>(value);
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
  const std::size_t page_size_u = static_cast<std::size_t>(page_size);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 14695981039346656037ULL;
    const std::size_t n = page_ids.size();
    for (std::size_t i = 0; i < n; ++i) {
      const std::size_t page_idx = static_cast<std::size_t>(page_ids[i]);
      const std::size_t slot_idx = static_cast<std::size_t>(slot_offsets[i]);
      const std::size_t linear_idx = page_idx * page_size_u + slot_idx;
      hash = mix(hash, cache[linear_idx]);
    }
  }
  return hash;
}