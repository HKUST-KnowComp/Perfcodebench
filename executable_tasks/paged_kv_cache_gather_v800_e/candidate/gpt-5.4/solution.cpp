#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int /*num_pages*/,
    int page_size,
    int iters) {
  const std::size_t n = page_ids.size();
  const uint32_t* const cache_ptr = cache.data();
  const uint32_t* const page_ptr = page_ids.data();
  const uint32_t* const slot_ptr = slot_offsets.data();
  const std::size_t page_sz = static_cast<std::size_t>(page_size);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;

    std::size_t i = 0;
    const std::size_t unroll = 4;
    const std::size_t limit = n & ~(unroll - 1);
    for (; i < limit; i += unroll) {
      h = mix(h, cache_ptr[static_cast<std::size_t>(page_ptr[i + 0]) * page_sz + static_cast<std::size_t>(slot_ptr[i + 0])]);
      h = mix(h, cache_ptr[static_cast<std::size_t>(page_ptr[i + 1]) * page_sz + static_cast<std::size_t>(slot_ptr[i + 1])]);
      h = mix(h, cache_ptr[static_cast<std::size_t>(page_ptr[i + 2]) * page_sz + static_cast<std::size_t>(slot_ptr[i + 2])]);
      h = mix(h, cache_ptr[static_cast<std::size_t>(page_ptr[i + 3]) * page_sz + static_cast<std::size_t>(slot_ptr[i + 3])]);
    }
    for (; i < n; ++i) {
      h = mix(h, cache_ptr[static_cast<std::size_t>(page_ptr[i]) * page_sz + static_cast<std::size_t>(slot_ptr[i])]);
    }

    hash = h;
  }
  return hash;
}
