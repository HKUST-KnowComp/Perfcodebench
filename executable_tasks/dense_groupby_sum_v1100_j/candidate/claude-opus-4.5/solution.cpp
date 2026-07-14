#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  std::vector<uint64_t> agg(static_cast<size_t>(groups));
  uint64_t out = 0;
  const size_t n = keys.size();
  const uint32_t* __restrict kptr = keys.data();
  const uint32_t* __restrict vptr = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset aggregation array
    for (int g = 0; g < groups; ++g) {
      agg[static_cast<size_t>(g)] = 0;
    }

    // Aggregate values by key
    for (size_t i = 0; i < n; ++i) {
      agg[kptr[i]] += vptr[i];
    }

    // Compute checksum using FNV-1a style hash
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
      hash ^= agg[static_cast<size_t>(g)];
      hash *= 1099511628211ULL;
    }
    out = hash;
  }
  return out;
}