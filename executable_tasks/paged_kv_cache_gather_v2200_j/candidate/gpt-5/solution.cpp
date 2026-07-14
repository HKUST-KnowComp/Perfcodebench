#include "interface.h"

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
    int /*num_pages*/,
    int page_size,
    int iters) {
  const std::size_t n = page_ids.size();

  // Precompute linear indices once
  std::vector<std::size_t> indices(n);
  for (std::size_t i = 0; i < n; ++i) {
    indices[i] = static_cast<std::size_t>(page_ids[i]) * static_cast<std::size_t>(page_size)
               + static_cast<std::size_t>(slot_offsets[i]);
  }

  const uint32_t* cachep = cache.data();
  const std::size_t* idxp = indices.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset hash each iteration to match baseline behavior
    hash = 1469598103934665603ULL;

    std::size_t i = 0;
    const std::size_t n4 = (n / 4) * 4;
    for (; i < n4; i += 4) {
      hash = mix(hash, static_cast<uint64_t>(cachep[idxp[i]]));
      hash = mix(hash, static_cast<uint64_t>(cachep[idxp[i + 1]]));
      hash = mix(hash, static_cast<uint64_t>(cachep[idxp[i + 2]]));
      hash = mix(hash, static_cast<uint64_t>(cachep[idxp[i + 3]]));
    }
    for (; i < n; ++i) {
      hash = mix(hash, static_cast<uint64_t>(cachep[idxp[i]]));
    }
  }

  return hash;
}
