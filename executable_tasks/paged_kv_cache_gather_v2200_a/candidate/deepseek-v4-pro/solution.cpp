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
  (void)num_pages;
  (void)iters;

  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = page_ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    std::size_t idx = static_cast<std::size_t>(page_ids[i]) * static_cast<std::size_t>(page_size) + static_cast<std::size_t>(slot_offsets[i]);
    hash = mix(hash, static_cast<uint64_t>(cache[idx]));
  }
  return hash;
}
