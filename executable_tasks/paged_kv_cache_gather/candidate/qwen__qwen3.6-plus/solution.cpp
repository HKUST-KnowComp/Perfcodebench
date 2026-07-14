#include "interface.h"
#include <cstdint>
#include <vector>

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
  const uint64_t prime = 1099511628211ULL;
  
  const std::size_t n = page_ids.size();
  const uint32_t* pid_ptr = page_ids.data();
  const uint32_t* off_ptr = slot_offsets.data();
  const uint32_t* cache_ptr = cache.data();
  const uint32_t ps = static_cast<uint32_t>(page_size);

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t val = cache_ptr[pid_ptr[i] * ps + off_ptr[i]];
    hash ^= val;
    hash *= prime;
  }
  return hash;
}
