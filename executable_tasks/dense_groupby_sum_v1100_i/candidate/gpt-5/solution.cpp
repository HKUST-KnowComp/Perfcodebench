#include "interface.h"

#include <cstring>
#include <vector>
#include <cstdint>

namespace {
inline uint64_t checksum_vec(const uint64_t* agg, int groups) {
  uint64_t hash = 1469598103934665603ULL;
  for (int g = 0; g < groups; ++g) {
    uint64_t v = agg[g];
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}
}  // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  if (groups <= 0) {
    // No groups to hash over; checksum remains the FNV offset basis regardless of input.
    return 1469598103934665603ULL;
  }

  std::vector<uint64_t> agg(static_cast<std::size_t>(groups));
  const std::size_t n = keys.size();
  const uint32_t* kptr = keys.data();
  const uint32_t* vptr = values.data();

  uint64_t out = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset aggregation array
    std::memset(agg.data(), 0, static_cast<std::size_t>(groups) * sizeof(uint64_t));

    // Accumulate
    std::size_t i = 0;
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    for (; i < n4; i += 4) {
      agg[kptr[i]]     += static_cast<uint64_t>(vptr[i]);
      agg[kptr[i + 1]] += static_cast<uint64_t>(vptr[i + 1]);
      agg[kptr[i + 2]] += static_cast<uint64_t>(vptr[i + 2]);
      agg[kptr[i + 3]] += static_cast<uint64_t>(vptr[i + 3]);
    }
    for (; i < n; ++i) {
      agg[kptr[i]] += static_cast<uint64_t>(vptr[i]);
    }

    // Compute checksum for this iteration
    out = checksum_vec(agg.data(), groups);
  }

  return out;
}
