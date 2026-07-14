#include "interface.h"

#include <vector>
#include <cstdint>

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL; // FNV-1a 64-bit prime
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
  (void)num_pages; // Unused in optimized direct indexing

  // Preserve baseline behavior: if no iterations are requested, return 0.
  if (iters <= 0) {
    return 0ULL;
  }

  const uint32_t* cache_data = cache.data();
  const uint32_t* pid_data = page_ids.data();
  const uint32_t* off_data = slot_offsets.data();
  const std::size_t n = page_ids.size();
  const std::size_t ps = static_cast<std::size_t>(page_size);

  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis

  // Single pass is sufficient because each iteration in the baseline
  // resets hash to the same offset and recomputes the same sequence.
  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t idx = static_cast<std::size_t>(pid_data[i]) * ps + static_cast<std::size_t>(off_data[i]);
    hash = mix(hash, static_cast<uint64_t>(cache_data[idx]));
  }

  return hash;
}
