#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  std::vector<uint64_t> agg(groups, 0);
  uint64_t out = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset accumulator
    uint64_t* data = agg.data();
    for (int g = 0; g < groups; ++g) {
      data[g] = 0;
    }
    // Aggregate
    const size_t n = keys.size();
    const uint32_t* k = keys.data();
    const uint32_t* v = values.data();
    for (size_t i = 0; i < n; ++i) {
      data[k[i]] += v[i];
    }
    // Checksum
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
      hash ^= data[g];
      hash *= 1099511628211ULL;
    }
    out = hash;
  }
  return out;
}
