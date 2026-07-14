#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int /*num_pages*/,
    int page_size,
    int /*iters*/) {
  const uint32_t* cache_ptr = cache.data();
  const uint32_t* pid_ptr = page_ids.data();
  const uint32_t* off_ptr = slot_offsets.data();
  const std::size_t n = page_ids.size();
  const std::size_t ps = static_cast<std::size_t>(page_size);

  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET_BASIS;

  std::size_t i = 0;
  const std::size_t n4 = n & static_cast<std::size_t>(~3ULL);

  for (; i < n4; i += 4) {
    std::size_t idx0 = static_cast<std::size_t>(pid_ptr[i]) * ps + static_cast<std::size_t>(off_ptr[i]);
    hash ^= static_cast<uint64_t>(cache_ptr[idx0]);
    hash *= FNV_PRIME;

    std::size_t idx1 = static_cast<std::size_t>(pid_ptr[i + 1]) * ps + static_cast<std::size_t>(off_ptr[i + 1]);
    hash ^= static_cast<uint64_t>(cache_ptr[idx1]);
    hash *= FNV_PRIME;

    std::size_t idx2 = static_cast<std::size_t>(pid_ptr[i + 2]) * ps + static_cast<std::size_t>(off_ptr[i + 2]);
    hash ^= static_cast<uint64_t>(cache_ptr[idx2]);
    hash *= FNV_PRIME;

    std::size_t idx3 = static_cast<std::size_t>(pid_ptr[i + 3]) * ps + static_cast<std::size_t>(off_ptr[i + 3]);
    hash ^= static_cast<uint64_t>(cache_ptr[idx3]);
    hash *= FNV_PRIME;
  }

  for (; i < n; ++i) {
    std::size_t idx = static_cast<std::size_t>(pid_ptr[i]) * ps + static_cast<std::size_t>(off_ptr[i]);
    hash ^= static_cast<uint64_t>(cache_ptr[idx]);
    hash *= FNV_PRIME;
  }

  return hash;
}
