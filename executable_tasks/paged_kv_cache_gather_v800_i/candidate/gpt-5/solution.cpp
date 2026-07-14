#include "interface.h"

namespace {
inline uint64_t fnv_mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;  // FNV-1a 64-bit prime
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
  // Match baseline behavior: if iters == 0, return 0 (hash never initialized inside loop).
  if (iters == 0) return 0ULL;

  const uint32_t* cdata = cache.data();
  const uint32_t* pids = page_ids.data();
  const uint32_t* offs = slot_offsets.data();
  const std::size_t n = page_ids.size();
  const std::size_t psize = static_cast<std::size_t>(page_size);

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis

  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t idx = static_cast<std::size_t>(pids[i]) * psize + static_cast<std::size_t>(offs[i]);
    const uint64_t val = static_cast<uint64_t>(cdata[idx]);
    hash = fnv_mix(hash, val);
  }

  return hash;
}
