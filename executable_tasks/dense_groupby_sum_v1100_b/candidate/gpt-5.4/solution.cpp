#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys,
                         const std::vector<uint32_t>& values,
                         int groups,
                         int iters) {
  const std::size_t n = keys.size();
  if (groups <= 0 || iters <= 0) {
    return 1469598103934665603ULL;
  }

  std::vector<uint64_t> sums(static_cast<std::size_t>(groups), 0);
  uint64_t out = 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (int g = 0; g < groups; ++g) {
      sums[static_cast<std::size_t>(g)] = 0;
    }

    const uint32_t* kptr = keys.data();
    const uint32_t* vptr = values.data();

    std::size_t i = 0;
    const std::size_t unroll_end = n & ~std::size_t(3);
    for (; i < unroll_end; i += 4) {
      sums[kptr[i + 0]] += static_cast<uint64_t>(vptr[i + 0]);
      sums[kptr[i + 1]] += static_cast<uint64_t>(vptr[i + 1]);
      sums[kptr[i + 2]] += static_cast<uint64_t>(vptr[i + 2]);
      sums[kptr[i + 3]] += static_cast<uint64_t>(vptr[i + 3]);
    }
    for (; i < n; ++i) {
      sums[kptr[i]] += static_cast<uint64_t>(vptr[i]);
    }

    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
      hash ^= sums[static_cast<std::size_t>(g)];
      hash *= 1099511628211ULL;
    }
    out = hash;
  }

  return out;
}
