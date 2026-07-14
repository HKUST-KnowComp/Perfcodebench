#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  std::vector<uint32_t> counts(bins, 0);
  const uint32_t* vdata = values.data();
  const size_t vsize = values.size();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Count only values within histogram range
    for (size_t i = 0; i < vsize; ++i) {
      const uint32_t v = vdata[i];
      if (v < bins) {
        ++counts[v];
      }
    }
    // Compute checksum and reset counts in one pass
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      const uint32_t c = counts[b];
      hash = mix(hash, c);
      counts[b] = 0;
    }
  }
  return hash;
}