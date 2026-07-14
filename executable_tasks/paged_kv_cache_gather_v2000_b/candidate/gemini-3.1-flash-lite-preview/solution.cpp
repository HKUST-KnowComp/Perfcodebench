#include "interface.h"
#include <vector>
#include <cstdint>

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
  uint64_t hash = 0;
  const size_t num_lookups = page_ids.size();
  
  // Pre-calculate base pointers for each page to avoid multiplication in the inner loop
  std::vector<const uint32_t*> page_ptrs(num_pages);
  for (int p = 0; p < num_pages; ++p) {
    page_ptrs[p] = &cache[static_cast<size_t>(p) * static_cast<size_t>(page_size)];
  }

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < num_lookups; ++i) {
      const uint32_t* page = page_ptrs[page_ids[i]];
      hash = mix(hash, page[slot_offsets[i]]);
    }
  }
  return hash;
}