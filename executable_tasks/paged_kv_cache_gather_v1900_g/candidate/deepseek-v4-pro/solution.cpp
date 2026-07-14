#include "interface.h"

#include <cstdint>

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
  if (iters == 0) {
    return 0;
  }

  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < page_ids.size(); ++i) {
    std::size_t idx = static_cast<std::size_t>(page_ids[i]) * static_cast<std::size_t>(page_size)
                      + static_cast<std::size_t>(slot_offsets[i]);
    hash = mix(hash, cache[idx]);
  }
  return hash;
}