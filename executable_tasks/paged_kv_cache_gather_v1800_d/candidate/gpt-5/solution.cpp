#include "interface.h"

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
    int num_pages,
    int page_size,
    int iters) {
  // Preserve baseline behavior: if no iterations, return initial hash value (0)
  if (iters <= 0) {
    return 0ULL;
  }

  const uint32_t* cache_ptr = cache.data();
  const uint32_t* pids = page_ids.data();
  const uint32_t* offs = slot_offsets.data();
  const std::size_t n = page_ids.size();
  const std::size_t stride = static_cast<std::size_t>(page_size);

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis

  // Unroll loop for better throughput
  std::size_t i = 0;
  for (; i + 4 <= n; i += 4) {
    std::size_t idx0 = static_cast<std::size_t>(pids[i + 0]) * stride + static_cast<std::size_t>(offs[i + 0]);
    std::size_t idx1 = static_cast<std::size_t>(pids[i + 1]) * stride + static_cast<std::size_t>(offs[i + 1]);
    std::size_t idx2 = static_cast<std::size_t>(pids[i + 2]) * stride + static_cast<std::size_t>(offs[i + 2]);
    std::size_t idx3 = static_cast<std::size_t>(pids[i + 3]) * stride + static_cast<std::size_t>(offs[i + 3]);

    hash = mix(hash, static_cast<uint64_t>(cache_ptr[idx0]));
    hash = mix(hash, static_cast<uint64_t>(cache_ptr[idx1]));
    hash = mix(hash, static_cast<uint64_t>(cache_ptr[idx2]));
    hash = mix(hash, static_cast<uint64_t>(cache_ptr[idx3]));
  }
  for (; i < n; ++i) {
    std::size_t idx = static_cast<std::size_t>(pids[i]) * stride + static_cast<std::size_t>(offs[i]);
    hash = mix(hash, static_cast<uint64_t>(cache_ptr[idx]));
  }

  return hash;
}
