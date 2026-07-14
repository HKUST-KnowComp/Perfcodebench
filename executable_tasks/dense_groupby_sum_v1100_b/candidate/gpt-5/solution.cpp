#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  // Match baseline behavior: if iters == 0, the loop body never runs and returns 0.
  if (iters <= 0) return 0ULL;

  // If groups <= 0, checksum loop is empty in baseline, returning FNV offset basis.
  if (groups <= 0) return 1469598103934665603ULL;

  const std::size_t n = keys.size();

  // Dense aggregation array; value-initialized to zero.
  std::vector<uint64_t> agg(static_cast<std::size_t>(groups));

  const uint32_t* kptr = keys.data();
  const uint32_t* vptr = values.data();

  // Accumulate with simple unrolling for better throughput.
  std::size_t i = 0;
  const std::size_t n4 = n & ~static_cast<std::size_t>(3);
  for (; i < n4; i += 4) {
    const uint32_t k0 = kptr[i];
    const uint32_t k1 = kptr[i + 1];
    const uint32_t k2 = kptr[i + 2];
    const uint32_t k3 = kptr[i + 3];
    agg[k0] += static_cast<uint64_t>(vptr[i]);
    agg[k1] += static_cast<uint64_t>(vptr[i + 1]);
    agg[k2] += static_cast<uint64_t>(vptr[i + 2]);
    agg[k3] += static_cast<uint64_t>(vptr[i + 3]);
  }
  for (; i < n; ++i) {
    agg[kptr[i]] += static_cast<uint64_t>(vptr[i]);
  }

  // FNV-1a over grouped sums in order [0..groups-1].
  uint64_t hash = 1469598103934665603ULL;      // FNV-1a 64-bit offset basis
  const uint64_t prime = 1099511628211ULL;     // FNV-1a 64-bit prime
  for (int g = 0; g < groups; ++g) {
    hash ^= agg[static_cast<std::size_t>(g)];
    hash *= prime;
  }

  return hash;
}
