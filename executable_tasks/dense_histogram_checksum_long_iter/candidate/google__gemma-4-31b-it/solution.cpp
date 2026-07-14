#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  // Use a vector as a direct-address table for the histogram to avoid map overhead.
  // Pre-allocate once outside the loop to avoid repeated allocations.
  std::vector<uint32_t> counts(bins, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts for the current iteration
    std::fill(counts.begin(), counts.end(), 0);

    // Accumulate histogram
    for (uint32_t v : values) {
      // The problem implies values are within [0, bins), but we check to be safe
      // if the contract allows values >= bins. Based on the baseline's logic,
      // only bins in [0, bins-1] contribute to the final hash.
      if (v < bins) {
        ++counts[v];
      }
    }

    // Compute checksum
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }
  }

  return hash;
}