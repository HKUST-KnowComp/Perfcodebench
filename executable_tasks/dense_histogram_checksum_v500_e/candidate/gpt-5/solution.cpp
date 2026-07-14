#include "interface.h"

namespace {
static constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
static constexpr uint64_t FNV_PRIME  = 1099511628211ULL;
}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  // Preserve baseline behavior for non-positive iters
  if (iters <= 0) return 0ULL;

  if (bins == 0u) return FNV_OFFSET;

  std::vector<uint32_t> counts(bins, 0u);

  const uint32_t nBins = bins;
  const uint32_t* vals = values.data();
  const size_t n = values.size();
  for (size_t i = 0; i < n; ++i) {
    uint32_t v = vals[i];
    if (v < nBins) {
      counts[v] += 1u;
    }
  }

  uint64_t hash = FNV_OFFSET;
  const uint32_t* c = counts.data();
  for (uint32_t b = 0; b < nBins; ++b) {
    hash ^= static_cast<uint64_t>(c[b]);
    hash *= FNV_PRIME;
  }

  return hash;
}
