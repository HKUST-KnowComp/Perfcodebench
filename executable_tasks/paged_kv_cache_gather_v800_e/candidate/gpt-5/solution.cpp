#include "interface.h"

#include <vector>

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
    int num_pages,
    int page_size,
    int iters) {
  // Preserve baseline semantics: if iters <= 0, return 0
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t n = page_ids.size();
  const std::size_t ps = static_cast<std::size_t>(page_size);

  const uint32_t* cache_ptr = cache.data();
  const uint32_t* pid_ptr = page_ids.data();
  const uint32_t* off_ptr = slot_offsets.data();

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis

  std::size_t i = 0;
  // Unroll by 4 for better ILP
  for (; i + 4 <= n; i += 4) {
    std::size_t idx0 = static_cast<std::size_t>(pid_ptr[i]) * ps + static_cast<std::size_t>(off_ptr[i]);
    hash = mix(hash, static_cast<uint64_t>(cache_ptr[idx0]));

    std::size_t idx1 = static_cast<std::size_t>(pid_ptr[i + 1]) * ps + static_cast<std::size_t>(off_ptr[i + 1]);
    hash = mix(hash, static_cast<uint64_t>(cache_ptr[idx1]));

    std::size_t idx2 = static_cast<std::size_t>(pid_ptr[i + 2]) * ps + static_cast<std::size_t>(off_ptr[i + 2]);
    hash = mix(hash, static_cast<uint64_t>(cache_ptr[idx2]));

    std::size_t idx3 = static_cast<std::size_t>(pid_ptr[i + 3]) * ps + static_cast<std::size_t>(off_ptr[i + 3]);
    hash = mix(hash, static_cast<uint64_t>(cache_ptr[idx3]));
  }
  for (; i < n; ++i) {
    std::size_t idx = static_cast<std::size_t>(pid_ptr[i]) * ps + static_cast<std::size_t>(off_ptr[i]);
    hash = mix(hash, static_cast<uint64_t>(cache_ptr[idx]));
  }

  return hash;
}
