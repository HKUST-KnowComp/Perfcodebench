#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  // Allocate aggregation buffer once; zero-initialized.
  std::vector<uint64_t> agg(groups, 0);
  uint64_t out = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Aggregate values by key (keys are assumed to be in [0, groups)).
    const std::size_t n = keys.size();
    for (std::size_t i = 0; i < n; ++i) {
      agg[keys[i]] += static_cast<uint64_t>(values[i]);
    }

    // Compute FNV-1a checksum over all groups and reset aggregation buffer.
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
      const uint64_t v = agg[g];
      hash ^= v;
      hash *= 1099511628211ULL;
      agg[g] = 0;  // reset for next iteration
    }
    out = hash;
  }

  return out;
}
