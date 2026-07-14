#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys,
                         const std::vector<uint32_t>& values,
                         int groups,
                         int iters) {
  const std::size_t n = keys.size();
  if (groups <= 0) {
    return 1469598103934665603ULL;
  }

  std::vector<uint64_t> agg(static_cast<std::size_t>(groups), 0);
  uint64_t out = 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (int g = 0; g < groups; ++g) {
      agg[static_cast<std::size_t>(g)] = 0;
    }

    const uint32_t* k = keys.data();
    const uint32_t* v = values.data();

    std::size_t i = 0;
    const std::size_t unroll_end = n & ~std::size_t(3);
    for (; i < unroll_end; i += 4) {
      agg[k[i + 0]] += static_cast<uint64_t>(v[i + 0]);
      agg[k[i + 1]] += static_cast<uint64_t>(v[i + 1]);
      agg[k[i + 2]] += static_cast<uint64_t>(v[i + 2]);
      agg[k[i + 3]] += static_cast<uint64_t>(v[i + 3]);
    }
    for (; i < n; ++i) {
      agg[k[i]] += static_cast<uint64_t>(v[i]);
    }

    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
      hash ^= agg[static_cast<std::size_t>(g)];
      hash *= 1099511628211ULL;
    }
    out = hash;
  }

  return out;
}
