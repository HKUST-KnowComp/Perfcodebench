#include "interface.h"
#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  // Use a vector on the stack/heap to store counts directly indexed by bin ID.
  // Since bins are bounded and dense, a flat array is optimal.
  std::vector<uint32_t> counts(bins);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts efficiently
    std::fill(counts.begin(), counts.end(), 0);

    // Accumulate counts
    for (uint32_t v : values) {
      if (v < bins) {
        counts[v]++;
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