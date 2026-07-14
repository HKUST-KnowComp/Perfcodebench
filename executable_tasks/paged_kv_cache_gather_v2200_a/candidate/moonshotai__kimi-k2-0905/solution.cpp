#include "interface.h"
#include <vector>
#include <cstdint>

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
    int iters) {
  const size_t page_size_u = static_cast<size_t>(page_size);
  uint64_t hash = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    const size_t n = page_ids.size();
    size_t i = 0;
    // unroll 4x
    for (; i + 3 < n; i += 4) {
      const size_t off0 = static_cast<size_t>(page_ids[i  ]) * page_size_u + static_cast<size_t>(slot_offsets[i  ]);
      const size_t off1 = static_cast<size_t>(page_ids[i+1]) * page_size_u + static_cast<size_t>(slot_offsets[i+1]);
      const size_t off2 = static_cast<size_t>(page_ids[i+2]) * page_size_u + static_cast<size_t>(slot_offsets[i+2]);
      const size_t off3 = static_cast<size_t>(page_ids[i+3]) * page_size_u + static_cast<size_t>(slot_offsets[i+3]);
      hash = mix(hash, cache[off0]);
      hash = mix(hash, cache[off1]);
      hash = mix(hash, cache[off2]);
      hash = mix(hash, cache[off3]);
    }
    // tail
    for (; i < n; ++i) {
      const size_t off = static_cast<size_t>(page_ids[i]) * page_size_u + static_cast<size_t>(slot_offsets[i]);
      hash = mix(hash, cache[off]);
    }
  }
  return hash;
}