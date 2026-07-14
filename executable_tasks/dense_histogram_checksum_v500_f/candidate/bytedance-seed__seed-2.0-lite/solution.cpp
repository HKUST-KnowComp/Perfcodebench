#include "interface.h"

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  std::vector<uint32_t> counts(bins, 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Accumulate values into dense histogram bins
    for (uint32_t v : values) {
      ++counts[v];
    }
    // Compute FNV-style checksum and reset counts for next iteration
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
      counts[b] = 0; // Zero immediately after use to avoid separate reset pass
    }
  }
  return hash;
}