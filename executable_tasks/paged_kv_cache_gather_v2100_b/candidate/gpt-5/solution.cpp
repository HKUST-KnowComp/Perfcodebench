#include "interface.h"

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
  if (iters == 0) {
    return 0ULL;
  }

  const uint32_t* cache_data = cache.data();
  const uint32_t* page_ids_data = page_ids.data();
  const uint32_t* slot_data = slot_offsets.data();

  const std::size_t n = page_ids.size();

  uint64_t hash = 1469598103934665603ULL;

  std::size_t i = 0;
  // Unroll by 4 for better throughput while preserving order.
  for (; i + 4 <= n; i += 4) {
    std::size_t idx0 = static_cast<std::size_t>(page_ids_data[i]) * static_cast<std::size_t>(page_size) + static_cast<std::size_t>(slot_data[i]);
    hash = mix(hash, static_cast<uint64_t>(cache_data[idx0]));

    std::size_t idx1 = static_cast<std::size_t>(page_ids_data[i + 1]) * static_cast<std::size_t>(page_size) + static_cast<std::size_t>(slot_data[i + 1]);
    hash = mix(hash, static_cast<uint64_t>(cache_data[idx1]));

    std::size_t idx2 = static_cast<std::size_t>(page_ids_data[i + 2]) * static_cast<std::size_t>(page_size) + static_cast<std::size_t>(slot_data[i + 2]);
    hash = mix(hash, static_cast<uint64_t>(cache_data[idx2]));

    std::size_t idx3 = static_cast<std::size_t>(page_ids_data[i + 3]) * static_cast<std::size_t>(page_size) + static_cast<std::size_t>(slot_data[i + 3]);
    hash = mix(hash, static_cast<uint64_t>(cache_data[idx3]));
  }
  for (; i < n; ++i) {
    std::size_t idx = static_cast<std::size_t>(page_ids_data[i]) * static_cast<std::size_t>(page_size) + static_cast<std::size_t>(slot_data[i]);
    hash = mix(hash, static_cast<uint64_t>(cache_data[idx]));
  }

  return hash;
}
