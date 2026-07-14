#include "interface.h"

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int /*num_pages*/,
    int page_size,
    int iters) {
  // Match baseline semantics: if iters == 0, return 0 without computing.
  if (iters == 0) {
    return 0ULL;
  }

  // FNV-1a 64-bit constants
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  const uint32_t* cache_ptr = cache.data();
  const uint32_t* pids = page_ids.data();
  const uint32_t* offs = slot_offsets.data();
  const std::size_t n = page_ids.size();
  const std::size_t ps = static_cast<std::size_t>(page_size);

  // If no indices to gather, return the offset basis (as baseline would per-iter).
  if (n == 0) {
    return FNV_OFFSET_BASIS;
  }

  uint64_t hash = FNV_OFFSET_BASIS;

  std::size_t i = 0;
  // Unroll by 4 for a bit more ILP
  for (; i + 4 <= n; i += 4) {
    std::size_t idx0 = static_cast<std::size_t>(pids[i + 0]) * ps + static_cast<std::size_t>(offs[i + 0]);
    std::size_t idx1 = static_cast<std::size_t>(pids[i + 1]) * ps + static_cast<std::size_t>(offs[i + 1]);
    std::size_t idx2 = static_cast<std::size_t>(pids[i + 2]) * ps + static_cast<std::size_t>(offs[i + 2]);
    std::size_t idx3 = static_cast<std::size_t>(pids[i + 3]) * ps + static_cast<std::size_t>(offs[i + 3]);

    hash ^= static_cast<uint64_t>(cache_ptr[idx0]);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(cache_ptr[idx1]);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(cache_ptr[idx2]);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(cache_ptr[idx3]);
    hash *= FNV_PRIME;
  }
  for (; i < n; ++i) {
    std::size_t idx = static_cast<std::size_t>(pids[i]) * ps + static_cast<std::size_t>(offs[i]);
    hash ^= static_cast<uint64_t>(cache_ptr[idx]);
    hash *= FNV_PRIME;
  }

  return hash;
}
