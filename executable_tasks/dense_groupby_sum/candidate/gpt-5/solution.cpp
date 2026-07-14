#include "interface.h"

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  if (iters == 0) return 0ULL;
  if (groups <= 0) return 1469598103934665603ULL;

  const std::size_t n = keys.size();
  const uint32_t gmax = static_cast<uint32_t>(groups);

  std::vector<uint64_t> sums(static_cast<std::size_t>(groups), 0ULL);
  uint64_t out = 0ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // Accumulate into dense array, ignoring out-of-range keys
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t k = keys[i];
      if (k < gmax) {
        sums[k] += static_cast<uint64_t>(values[i]);
      }
    }

    // Compute FNV-1a checksum across groups and reset sums for next iteration
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
      uint64_t v = sums[static_cast<std::size_t>(g)];
      hash ^= v;
      hash *= 1099511628211ULL;
      sums[static_cast<std::size_t>(g)] = 0ULL;
    }
    out = hash;
  }

  return out;
}
