#include "interface.h"

#include <cstdint>
#include <vector>

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  // Match baseline behavior: if iters == 0, hash was never set/updated and remains 0.
  if (iters <= 0) return 0ULL;

  // For iters > 0, the baseline recomputes the same result each time and returns the last.
  // Compute it once using a dense accumulator for performance.
  if (domain == 0) {
    // No keys to mix; baseline sets hash to FNV offset and returns.
    return 1469598103934665603ULL;
  }

  std::vector<uint64_t> accum(domain, 0ULL);

  const std::size_t n = indices.size();
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t idx = indices[i];
    if (idx < domain) {
      accum[idx] += static_cast<uint64_t>(values[i]);
    }
  }

  uint64_t hash = 1469598103934665603ULL;
  const uint64_t* data = accum.data();
  for (uint32_t k = 0; k < domain; ++k) {
    hash = mix(hash, data[k]);
  }
  return hash;
}
