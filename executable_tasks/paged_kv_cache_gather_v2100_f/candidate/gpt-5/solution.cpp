#include "interface.h"

namespace {
constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime  = 1099511628211ULL;
}

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int /*num_pages*/,    // not needed for computation here
    int page_size,
    int iters) {
  // Match baseline behavior: if iters == 0, return initial hash (0)
  if (iters <= 0) {
    return 0ULL;
  }

  const uint32_t* cache_ptr = cache.data();
  const uint32_t* ids_ptr = page_ids.data();
  const uint32_t* offs_ptr = slot_offsets.data();
  const std::size_t n = page_ids.size();
  const std::size_t ps = static_cast<std::size_t>(page_size);

  uint64_t hash = kFNVOffset;

  // Unrolled loop for better throughput
  std::size_t i = 0;
  const std::size_t n4 = n & ~static_cast<std::size_t>(3);
  for (; i < n4; i += 4) {
    std::size_t idx0 = static_cast<std::size_t>(ids_ptr[i + 0]) * ps + static_cast<std::size_t>(offs_ptr[i + 0]);
    std::size_t idx1 = static_cast<std::size_t>(ids_ptr[i + 1]) * ps + static_cast<std::size_t>(offs_ptr[i + 1]);
    std::size_t idx2 = static_cast<std::size_t>(ids_ptr[i + 2]) * ps + static_cast<std::size_t>(offs_ptr[i + 2]);
    std::size_t idx3 = static_cast<std::size_t>(ids_ptr[i + 3]) * ps + static_cast<std::size_t>(offs_ptr[i + 3]);

    hash ^= static_cast<uint64_t>(cache_ptr[idx0]);
    hash *= kFNVPrime;
    hash ^= static_cast<uint64_t>(cache_ptr[idx1]);
    hash *= kFNVPrime;
    hash ^= static_cast<uint64_t>(cache_ptr[idx2]);
    hash *= kFNVPrime;
    hash ^= static_cast<uint64_t>(cache_ptr[idx3]);
    hash *= kFNVPrime;
  }
  for (; i < n; ++i) {
    std::size_t idx = static_cast<std::size_t>(ids_ptr[i]) * ps + static_cast<std::size_t>(offs_ptr[i]);
    hash ^= static_cast<uint64_t>(cache_ptr[idx]);
    hash *= kFNVPrime;
  }

  return hash;
}
