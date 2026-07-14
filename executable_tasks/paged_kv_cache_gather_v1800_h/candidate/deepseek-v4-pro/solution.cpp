#include "interface.h"

#include <cstddef>

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
  const size_t page_size_s = static_cast<size_t>(page_size);
  const size_t num_pages_s = static_cast<size_t>(num_pages);
  const uint32_t* cache_data = cache.data();

  // precompute start pointers for each page
  std::vector<const uint32_t*> page_start(num_pages_s);
  for (size_t p = 0; p < num_pages_s; ++p) {
    page_start[p] = cache_data + p * page_size_s;
  }

  const size_t N = page_ids.size();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < N; ++i) {
      const uint32_t* page_ptr = page_start[static_cast<size_t>(page_ids[i])];
      uint64_t val = static_cast<uint64_t>(page_ptr[static_cast<size_t>(slot_offsets[i])]);
      hash ^= val;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}