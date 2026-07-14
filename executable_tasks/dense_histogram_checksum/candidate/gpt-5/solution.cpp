#include "interface.h"

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int /*iters*/) {
  // Fast path: no bins means the hash remains the FNV offset basis.
  if (bins == 0u) {
    return 1469598103934665603ULL;
  }

  // Dense histogram: only count values within [0, bins).
  std::vector<uint32_t> counts(bins, 0u);
  for (uint32_t v : values) {
    if (v < bins) {
      ++counts[v];
    }
  }

  // Compute checksum using the same mixing as the baseline.
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t b = 0; b < bins; ++b) {
    hash = mix(hash, counts[b]);
  }

  return hash;
}
