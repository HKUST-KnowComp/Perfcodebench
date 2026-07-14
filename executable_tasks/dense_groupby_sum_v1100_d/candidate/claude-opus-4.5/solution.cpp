#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  std::vector<uint64_t> agg(static_cast<size_t>(groups));
  uint64_t out = 0;
  const size_t n = keys.size();
  const uint32_t* __restrict k = keys.data();
  const uint32_t* __restrict v = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    // Clear aggregation buffer
    uint64_t* __restrict a = agg.data();
    for (int g = 0; g < groups; ++g) {
      a[g] = 0;
    }

    // Aggregate: direct index into dense array
    for (size_t i = 0; i < n; ++i) {
      a[k[i]] += v[i];
    }

    // Compute checksum (FNV-1a style)
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
      hash ^= a[g];
      hash *= 1099511628211ULL;
    }
    out = hash;
  }
  return out;
}