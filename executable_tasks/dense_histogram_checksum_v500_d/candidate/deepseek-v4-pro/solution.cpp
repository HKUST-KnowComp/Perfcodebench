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
  std::vector<uint32_t> counts(bins, 0);
  for (int iter = 0; iter < iters; ++iter) {
    // count values within bin range
    for (uint32_t v : values) {
      if (v < bins) {
        ++counts[v];
      }
    }
    // compute hash and simultaneously clear counts for next iteration
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
      counts[b] = 0;
    }
  }
  return hash;
}