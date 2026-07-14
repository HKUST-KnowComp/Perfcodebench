#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  if (iters <= 0) return 0;
  
  // Precompute histogram: count of each bin (only values < bins)
  std::vector<uint32_t> counts(bins, 0);
  for (uint32_t v : values) {
    if (v < bins) {
      ++counts[v];
    }
  }

  const uint32_t* counts_data = counts.data();
  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts_data[b]);
    }
  }
  return hash;
}