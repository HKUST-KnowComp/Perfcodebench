#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // anonymous namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  std::vector<uint32_t> counts(bins, 0);
  uint64_t result = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Zero out counts
    for (uint32_t& c : counts) {
      c = 0;
    }

    // Accumulate histogram
    for (uint32_t v : values) {
      if (v < bins) {
        ++counts[v];
      }
    }

    // Compute hash over bin counts
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }
    result = hash;
  }

  return result;
}
