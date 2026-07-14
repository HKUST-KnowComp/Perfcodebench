#include "interface.h"

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;
}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  if (iters <= 0) return 0ULL;

  std::vector<uint32_t> hist(bins, 0u);
  uint64_t hash = 0ULL;
  const uint32_t bins_local = bins;

  for (int iter = 0; iter < iters; ++iter) {
    // Accumulate counts for values within [0, bins)
    for (uint32_t v : values) {
      if (v < bins_local) ++hist[v];
    }

    // Compute checksum (FNV-1a style mix) and reset histogram for next iteration
    hash = FNV_OFFSET;
    uint32_t* p = hist.data();
    const uint32_t* end = p + bins_local;
    for (; p != end; ++p) {
      hash ^= static_cast<uint64_t>(*p);
      hash *= FNV_PRIME;
      *p = 0u; // reset for next iteration
    }
  }

  return hash;
}
