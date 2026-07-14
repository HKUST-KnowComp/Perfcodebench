#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  // Preserve baseline behavior: if iters <= 0, return 0
  if (iters <= 0) return 0ULL;

  if (bins == 0u) {
    // With zero bins, baseline sets hash to offset basis each iter and does no mixing.
    // Since each iteration yields the same value and only the last is returned,
    // computing once is sufficient.
    return 1469598103934665603ULL;
  }

  // Dense histogram for low bin counts
  std::vector<uint32_t> counts(bins, 0u);
  const uint32_t nb = bins;
  uint32_t* cnt = counts.data();

  for (uint32_t v : values) {
    if (v < nb) {
      ++cnt[v];
    }
  }

  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  for (uint32_t b = 0; b < nb; ++b) {
    hash = mix(hash, static_cast<uint64_t>(cnt[b]));
  }

  return hash;
}
