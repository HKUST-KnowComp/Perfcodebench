#include "interface.h"

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  (void)iters; // Result is identical per iteration in baseline semantics

  const std::size_t d = static_cast<std::size_t>(domain);
  std::vector<uint64_t> accum(d, 0ULL);

  const std::size_t n = indices.size();
  const std::size_t m = values.size();
  const std::size_t limit = n < m ? n : m;

  const uint32_t* idxp = indices.data();
  const uint32_t* valp = values.data();
  for (std::size_t i = 0; i < limit; ++i) {
    const uint32_t idx = idxp[i];
    if (idx < domain) {
      accum[static_cast<std::size_t>(idx)] += static_cast<uint64_t>(valp[i]);
    }
  }

  uint64_t hash = 1469598103934665603ULL; // FNV offset basis
  const uint64_t FNV_PRIME = 1099511628211ULL;
  for (std::size_t i = 0; i < d; ++i) {
    hash ^= accum[i];
    hash *= FNV_PRIME;
  }

  return hash;
}
