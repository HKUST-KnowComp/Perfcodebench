#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t checksum_dense(const uint64_t* sums, int groups) {
  uint64_t hash = 1469598103934665603ULL;
  for (int g = 0; g < groups; ++g) {
    hash ^= sums[g];
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
  if (groups <= 0) {
    return 1469598103934665603ULL;
  }

  std::vector<uint64_t> sums(static_cast<std::size_t>(groups), 0);
  uint64_t out = 0;

  const uint32_t* k = keys.data();
  const uint32_t* v = values.data();
  uint64_t* s = sums.data();

  for (int iter = 0; iter < iters; ++iter) {
    for (int g = 0; g < groups; ++g) {
      s[g] = 0;
    }

    std::size_t i = 0;
    const std::size_t unroll = 4;
    const std::size_t limit = n & ~(unroll - 1);
    for (; i < limit; i += unroll) {
      s[k[i]] += static_cast<uint64_t>(v[i]);
      s[k[i + 1]] += static_cast<uint64_t>(v[i + 1]);
      s[k[i + 2]] += static_cast<uint64_t>(v[i + 2]);
      s[k[i + 3]] += static_cast<uint64_t>(v[i + 3]);
    }
    for (; i < n; ++i) {
      s[k[i]] += static_cast<uint64_t>(v[i]);
    }

    out = checksum_dense(s, groups);
  }

  return out;
}
