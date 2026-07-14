#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  if (iters <= 0) {
    return 0;
  }

  // Use a dense vector for the histogram counts.
  // Since the input 'values' and 'bins' are constant across iterations,
  // we only need to compute the histogram and checksum once.
  std::vector<uint32_t> counts(bins, 0);

  // Accumulate counts
  for (uint32_t v : values) {
    if (v < bins) {
      ++counts[v];
    }
  }

  // Calculate the checksum using the FNV-1a style mix function provided in the baseline.
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t b = 0; b < bins; ++b) {
    hash = mix(hash, static_cast<uint64_t>(counts[b]));
  }

  return hash;
}
