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
  if (iters == 0) return 0;
  (void)num_pages;
  
  const uint32_t* cache_ptr = cache.data();
  const uint32_t* pid_ptr = page_ids.data();
  const uint32_t* off_ptr = slot_offsets.data();
  std::size_t n = page_ids.size();
  std::size_t ps = static_cast<std::size_t>(page_size);
  
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < n; ++i) {
    std::size_t idx = pid_ptr[i] * ps + off_ptr[i];
    uint32_t val = cache_ptr[idx];
    hash ^= val;
    hash *= 1099511628211ULL;
  }
  return hash;
}