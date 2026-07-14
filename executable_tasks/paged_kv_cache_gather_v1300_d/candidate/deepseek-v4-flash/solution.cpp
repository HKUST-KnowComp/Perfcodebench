#include "interface.h"
#include <cstdint>
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
  const std::size_t n = page_ids.size();
  // Precompute linear indices into cache
  std::vector<std::size_t> indices(n);
  const std::size_t page_size_sz = static_cast<std::size_t>(page_size);
  for (std::size_t i = 0; i < n; ++i) {
    indices[i] = static_cast<std::size_t>(page_ids[i]) * page_size_sz + static_cast<std::size_t>(slot_offsets[i]);
  }
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL; // FNV-1a offset basis
    for (std::size_t i = 0; i < n; ++i) {
      hash = mix(hash, cache[indices[i]]);
    }
  }
  return hash;
}
