#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t checksum_dense(const std::vector<uint64_t>& agg, int groups) {
  uint64_t hash = 1469598103934665603ULL;
  for (int g = 0; g < groups; ++g) {
    hash ^= agg[static_cast<std::size_t>(g)];
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys,
                          const std::vector<uint32_t>& values,
                          int groups,
                          int iters) {
  const std::size_t n = keys.size();
  std::vector<uint64_t> agg(static_cast<std::size_t>(groups), 0);

  uint64_t out = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int g = 0; g < groups; ++g) {
      agg[static_cast<std::size_t>(g)] = 0;
    }

    const uint32_t* k = keys.data();
    const uint32_t* v = values.data();
    for (std::size_t i = 0; i < n; ++i) {
      agg[k[i]] += static_cast<uint64_t>(v[i]);
    }

    out = checksum_dense(agg, groups);
  }
  return out;
}
