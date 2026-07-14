#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  // Use a dense array for the histogram to avoid unordered_map overhead.
  // Pre-allocate to avoid repeated allocations inside the loop.
  std::vector<uint32_t> counts(bins, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts for the current iteration
    std::fill(counts.begin(), counts.end(), 0);

    for (uint32_t v : values) {
      // The problem implies values are bounded by 'bins' based on the checksum loop
      if (v < bins) {
        ++counts[v];
      }
    }

    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }
  }
  return hash;
}