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
  uint64_t hash = 0;
  const size_t n = page_ids.size();
  const size_t ps = static_cast<size_t>(page_size);
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      uint64_t val = cache[static_cast<size_t>(page_ids[i]) * ps + static_cast<size_t>(slot_offsets[i])];
      hash = mix(hash, val);
    }
  }
  return hash;
}
