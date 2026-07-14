#include "interface.h"

#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  const size_t n = keys.size();
  const uint32_t* kptr = keys.data();
  const uint32_t* vptr = values.data();

  std::vector<uint64_t> agg_vec(static_cast<size_t>(groups), 0ULL);
  uint64_t* agg = agg_vec.data();

  uint64_t out = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Accumulate sums per key
    size_t i = 0;
    const size_t n4 = n & ~static_cast<size_t>(3);
    for (; i < n4; i += 4) {
      agg[kptr[i    ]] += vptr[i    ];
      agg[kptr[i + 1]] += vptr[i + 1];
      agg[kptr[i + 2]] += vptr[i + 2];
      agg[kptr[i + 3]] += vptr[i + 3];
    }
    for (; i < n; ++i) {
      agg[kptr[i]] += vptr[i];
    }

    // FNV-1a checksum and clear for next iteration in one pass
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
      const uint64_t v = agg[g];
      hash ^= v;
      hash *= 1099511628211ULL;
      agg[g] = 0ULL;
    }
    out = hash;
  }

  return out;
}
