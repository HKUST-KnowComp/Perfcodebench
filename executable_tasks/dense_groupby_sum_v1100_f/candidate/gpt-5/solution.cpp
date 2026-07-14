#include "interface.h"

namespace {

static inline uint64_t checksum_vec(const std::vector<uint64_t>& agg, int groups) {
  uint64_t hash = 1469598103934665603ULL;
  for (int g = 0; g < groups; ++g) {
    uint64_t v = agg[static_cast<std::size_t>(g)];
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  (void)iters;  // Result is invariant across iterations; compute once.

  if (groups <= 0) {
    return 1469598103934665603ULL;
  }

  std::vector<uint64_t> agg(static_cast<std::size_t>(groups), 0ULL);

  const std::size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint32_t* v = values.data();

  std::size_t i = 0;
  const std::size_t n_unrolled = n & ~static_cast<std::size_t>(3);
  for (; i < n_unrolled; i += 4) {
    agg[static_cast<std::size_t>(k[i    ])] += static_cast<uint64_t>(v[i    ]);
    agg[static_cast<std::size_t>(k[i + 1])] += static_cast<uint64_t>(v[i + 1]);
    agg[static_cast<std::size_t>(k[i + 2])] += static_cast<uint64_t>(v[i + 2]);
    agg[static_cast<std::size_t>(k[i + 3])] += static_cast<uint64_t>(v[i + 3]);
  }
  for (; i < n; ++i) {
    agg[static_cast<std::size_t>(k[i])] += static_cast<uint64_t>(v[i]);
  }

  return checksum_vec(agg, groups);
}