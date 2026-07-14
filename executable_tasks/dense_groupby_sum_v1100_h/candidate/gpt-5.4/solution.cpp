#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys,
                         const std::vector<uint32_t>& values,
                         int groups,
                         int iters) {
  const std::size_t n = keys.size();
  if (groups <= 0 || iters <= 0) {
    return 0;
  }

  std::vector<uint64_t> agg(static_cast<std::size_t>(groups));
  uint64_t out = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Zero the dense aggregation buffer.
    for (int g = 0; g < groups; ++g) {
      agg[static_cast<std::size_t>(g)] = 0;
    }

    // Aggregate directly by dense key.
    for (std::size_t i = 0; i < n; ++i) {
      agg[keys[i]] += static_cast<uint64_t>(values[i]);
    }

    // Compute checksum in group order, matching reference semantics.
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
      hash ^= agg[static_cast<std::size_t>(g)];
      hash *= 1099511628211ULL;
    }
    out = hash;
  }

  return out;
}
