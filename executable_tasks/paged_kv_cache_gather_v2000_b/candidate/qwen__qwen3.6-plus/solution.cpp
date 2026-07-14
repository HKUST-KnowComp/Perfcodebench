#include "interface.h"
#include <vector>
#include <cstddef>

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  (void)num_pages;
  const size_t n = page_ids.size();
  if (n == 0 || iters == 0) return 0;

  std::vector<size_t> indices(n);
  const size_t ps = static_cast<size_t>(page_size);
  for (size_t i = 0; i < n; ++i) {
    indices[i] = static_cast<size_t>(page_ids[i]) * ps + slot_offsets[i];
  }

  const uint32_t* cache_ptr = cache.data();
  const size_t* idx_ptr = indices.data();
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      hash ^= cache_ptr[idx_ptr[i]];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}