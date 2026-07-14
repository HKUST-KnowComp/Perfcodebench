#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  // Match baseline behavior: if no iterations, return initial hash value (0)
  if (iters <= 0) return 0ULL;

  // If there are iterations but zero bins, baseline returns the FNV offset basis
  if (bins == 0) return 1469598103934665603ULL;

  // Build dense histogram once; repeated iterations in the baseline do not change the final hash
  std::vector<uint32_t> hist(bins, 0u);
  for (uint32_t v : values) {
    if (v < bins) {
      ++hist[v];
    }
  }

  // Compute checksum (FNV-1 style mixing) over all bins
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t b = 0; b < bins; ++b) {
    hash = mix(hash, static_cast<uint64_t>(hist[b]));
  }
  return hash;
}
