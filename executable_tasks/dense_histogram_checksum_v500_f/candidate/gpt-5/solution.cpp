#include "interface.h"

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  // If bins == 0 and iters > 0, the baseline would set hash to the seed and mix zero times.
  if (bins == 0u) {
    return 1469598103934665603ULL;
  }

  // Dense histogram over [0, bins).
  std::vector<uint32_t> counts(bins, 0u);
  const uint32_t bmax = bins;  // local copy to enable compiler optimizations

  const uint32_t* pv = values.empty() ? nullptr : values.data();
  const uint32_t* pv_end = pv + values.size();
  for (const uint32_t* p = pv; p != pv_end; ++p) {
    uint32_t v = *p;
    if (v < bmax) {
      ++counts[v];
    }
  }

  // FNV-1a 64-bit
  const uint64_t fnv_offset_basis = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;
  uint64_t hash = fnv_offset_basis;

  const uint32_t* pc = counts.data();
  const uint32_t* pc_end = pc + bins;
  for (const uint32_t* p = pc; p != pc_end; ++p) {
    hash ^= static_cast<uint64_t>(*p);
    hash *= fnv_prime;
  }

  return hash;
}
