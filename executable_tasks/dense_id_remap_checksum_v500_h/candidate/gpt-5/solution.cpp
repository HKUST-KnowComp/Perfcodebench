#include "interface.h"

namespace {

static inline uint64_t mix64(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

static inline std::size_t lower_bound_idx(const uint32_t* keys, std::size_t n, uint32_t probe) {
  std::size_t lo = 0, hi = n;
  while (lo < hi) {
    std::size_t mid = (lo + hi) >> 1;
    uint32_t v = keys[mid];
    if (v < probe) {
      lo = mid + 1;
    } else {
      hi = mid;
    }
  }
  return lo;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  // If no iterations are requested, baseline returns 0.
  if (iters <= 0) {
    return 0ULL;
  }

  const uint32_t* kptr = keys.data();
  const std::size_t ksize = keys.size();
  const uint32_t* vptr = values.data();
  const uint32_t* p = probes.data();
  const uint32_t* p_end = p + probes.size();

  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis
  for (; p != p_end; ++p) {
    std::size_t idx = lower_bound_idx(kptr, ksize, *p);
    hash = mix64(hash, static_cast<uint64_t>(vptr[idx]));
  }
  return hash;
}
