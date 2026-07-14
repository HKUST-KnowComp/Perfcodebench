#include "interface.h"
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values,
                            uint32_t bins,
                            int iters) {
  std::vector<uint32_t> counts(bins, 0u);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // reset histogram
    std::fill(counts.begin(), counts.end(), 0u);

    // accumulate
    for (uint32_t v : values) {
      if (v < bins) ++counts[v];
    }

    // compute checksum
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }
  }
  return hash;
}