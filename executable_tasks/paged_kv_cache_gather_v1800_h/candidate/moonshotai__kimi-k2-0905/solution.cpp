#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters)
{
  const std::size_t page_size_u = static_cast<std::size_t>(page_size);
  const std::size_t n = page_ids.size();
  uint64_t hash = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // unroll gather loop 4× to hide mix latency
    std::size_t i = 0;
    for (; i + 3 < n; i += 4) {
      const std::size_t idx0 = static_cast<std::size_t>(page_ids[i+0]) * page_size_u + static_cast<std::size_t>(slot_offsets[i+0]);
      const std::size_t idx1 = static_cast<std::size_t>(page_ids[i+1]) * page_size_u + static_cast<std::size_t>(slot_offsets[i+1]);
      const std::size_t idx2 = static_cast<std::size_t>(page_ids[i+2]) * page_size_u + static_cast<std::size_t>(slot_offsets[i+2]);
      const std::size_t idx3 = static_cast<std::size_t>(page_ids[i+3]) * page_size_u + static_cast<std::size_t>(slot_offsets[i+3]);
      hash = mix(hash, cache[idx0]);
      hash = mix(hash, cache[idx1]);
      hash = mix(hash, cache[idx2]);
      hash = mix(hash, cache[idx3]);
    }
    // tail
    for (; i < n; ++i) {
      const std::size_t idx = static_cast<std::size_t>(page_ids[i]) * page_size_u + static_cast<std::size_t>(slot_offsets[i]);
      hash = mix(hash, cache[idx]);
    }
  }
  return hash;
}