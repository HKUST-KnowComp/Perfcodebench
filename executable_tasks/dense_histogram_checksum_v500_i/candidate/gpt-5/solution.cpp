#include "interface.h"

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;
}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  if (iters <= 0) return 0ULL;

  std::vector<uint32_t> counts(bins, 0u);
  uint64_t hash = 0ULL;

  for (int iter = 0; iter < iters; ++iter) {
    if (bins != 0) {
      uint32_t* c = counts.data();
      // Accumulate counts only for values within [0, bins)
      for (uint32_t v : values) {
        if (v < bins) {
          ++c[v];
        }
      }

      // Compute checksum and reset counts for next iteration
      uint64_t h = FNV_OFFSET;
      uint32_t* p = c;
      uint32_t* end = c + bins;
      for (; p != end; ++p) {
        h ^= static_cast<uint64_t>(*p);
        h *= FNV_PRIME;
        *p = 0u;
      }
      hash = h;
    } else {
      // No bins: checksum is the FNV offset basis each iteration
      hash = FNV_OFFSET;
    }
  }

  return hash;
}
