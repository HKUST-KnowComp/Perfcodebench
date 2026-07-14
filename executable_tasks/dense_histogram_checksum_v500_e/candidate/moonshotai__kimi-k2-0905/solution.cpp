#include "interface.h"

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
  // Allocate and zero the histogram once.
  std::vector<uint32_t> hist(bins, 0u);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset histogram to zero in one fast call.
    std::memset(hist.data(), 0, bins * sizeof(uint32_t));

    // Accumulate counts.
    for (uint32_t v : values) {
      if (v < bins) {
        ++hist[v];
      }
    }

    // Compute checksum over the dense histogram.
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, hist[b]);
    }
  }
  return hash;
}