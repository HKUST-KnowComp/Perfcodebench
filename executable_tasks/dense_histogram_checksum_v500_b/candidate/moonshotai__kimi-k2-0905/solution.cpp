#include "interface.h"
#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t histogram_checksum(const std::vector<uint32_t>& values,
                            uint32_t bins,
                            int iters) {
  // Reusable dense histogram buffer
  std::vector<uint32_t> counts(bins, 0u);

  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset histogram to zero
    std::memset(counts.data(), 0, bins * sizeof(uint32_t));

    // Accumulate counts
    for (uint32_t v : values) {
      ++counts[v];
    }

    // Compute checksum over all bins
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }
  }
  return hash;
}