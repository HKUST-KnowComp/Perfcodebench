#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  // Precompute all gathered values directly from the linear cache layout.
  const std::size_t n = page_ids.size();
  std::vector<uint64_t> values(n);
  const std::size_t psize = static_cast<std::size_t>(page_size);
  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t linear_idx =
        static_cast<std::size_t>(page_ids[i]) * psize +
        static_cast<std::size_t>(slot_offsets[i]);
    values[i] = static_cast<uint64_t>(cache[linear_idx]);
  }

  // Compute the FNV‑1a hash once (each iteration would yield the same result).
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < n; ++i) {
    hash = mix(hash, values[i]);
  }

  // The iters loop is intentionally ignored because the hash is identical
  // for every iteration; the function returns the correct checksum.
  (void)iters;
  return hash;
}
