#include "interface.h"

#include <cstdint>
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
  // Precompute flattened indices for all gather operations
  const std::size_t N = page_ids.size();
  std::vector<std::size_t> indices(N);
  const std::size_t page_sz = static_cast<std::size_t>(page_size);
  for (std::size_t i = 0; i < N; ++i) {
    indices[i] = static_cast<std::size_t>(page_ids[i]) * page_sz +
                 static_cast<std::size_t>(slot_offsets[i]);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < N; ++i) {
      hash = mix(hash, cache[indices[i]]);
    }
  }
  return hash;
}