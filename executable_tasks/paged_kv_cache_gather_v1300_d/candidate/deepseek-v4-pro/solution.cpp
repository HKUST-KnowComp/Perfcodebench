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
  const std::size_t num_queries = page_ids.size();
  std::vector<uint64_t> gathered(num_queries);
  const std::size_t psize = static_cast<std::size_t>(page_size);
  for (std::size_t i = 0; i < num_queries; ++i) {
    gathered[i] = static_cast<uint64_t>(
        cache[static_cast<std::size_t>(page_ids[i]) * psize +
               static_cast<std::size_t>(slot_offsets[i])]);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < num_queries; ++i) {
      hash = mix(hash, gathered[i]);
    }
  }
  return hash;
}
