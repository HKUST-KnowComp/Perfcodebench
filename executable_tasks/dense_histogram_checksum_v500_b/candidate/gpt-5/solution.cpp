#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int /*iters*/) {
  // If there are zero bins, the baseline sets the hash to the FNV offset and does no mixing.
  if (bins == 0u) {
    return 1469598103934665603ULL;
  }

  // Build dense histogram for bins in [0, bins-1]. Values outside this range are ignored
  // (baseline used a map but only read keys 0..bins-1).
  std::vector<uint32_t> counts(bins, 0u);
  for (uint32_t v : values) {
    if (v < bins) {
      ++counts[v];
    }
  }

  // Compute checksum (FNV-like mix as in baseline).
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t b = 0; b < bins; ++b) {
    hash = mix(hash, static_cast<uint64_t>(counts[b]));
  }
  return hash;
}
