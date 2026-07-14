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
  if (iters <= 0) {
    return 0;
  }

  const uint32_t* c = cache.data();
  const uint32_t* p = page_ids.data();
  const uint32_t* s = slot_offsets.data();
  std::size_t n = page_ids.size();
  std::size_t ps = static_cast<std::size_t>(page_size);

  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= c[static_cast<std::size_t>(p[i]) * ps + static_cast<std::size_t>(s[i])];
    hash *= 1099511628211ULL;
  }

  return hash;
}
