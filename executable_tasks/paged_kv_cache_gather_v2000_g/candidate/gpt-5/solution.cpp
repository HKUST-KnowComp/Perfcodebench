#include "interface.h"

namespace {
inline uint64_t mix64(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int /*num_pages*/,  // unused in optimized path
    int page_size,
    int iters) {
  // Match baseline behavior: if iters == 0, return 0 (hash never initialized)
  if (iters <= 0) return 0ULL;

  const size_t n = page_ids.size();
  const uint32_t* pid = page_ids.data();
  const uint32_t* off = slot_offsets.data();
  const uint32_t* base = cache.data();
  const size_t psize = static_cast<size_t>(page_size);

  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

  for (size_t i = 0; i < n; ++i) {
    const size_t idx = static_cast<size_t>(pid[i]) * psize + static_cast<size_t>(off[i]);
    hash = mix64(hash, static_cast<uint64_t>(base[idx]));
  }

  return hash;
}
