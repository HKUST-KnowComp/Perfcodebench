#include "interface.h"
#include <vector>
#include <cstring>

namespace {

/**
 * FNV-1a style mixing function used for checksum calculation.
 */
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

/**
 * Optimized dense histogram accumulation.
 * 
 * The baseline used std::unordered_map which is slow due to hashing and pointer chasing.
 * Since the histogram is dense and we iterate from 0 to bins-1, a contiguous array
 * is significantly faster and cache-friendly.
 */
uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  // Pre-allocate buffer to avoid repeated allocations.
  std::vector<uint32_t> counts(bins);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Efficiently clear the histogram buffer.
    if (bins > 0) {
      std::memset(counts.data(), 0, bins * sizeof(uint32_t));
    }

    // Accumulate counts. 
    // The baseline logic effectively ignores values >= bins because the checksum 
    // loop only looks at indices [0, bins-1].
    for (uint32_t v : values) {
      if (v < bins) {
        ++counts[v];
      }
    }

    // Initialize hash with the FNV offset basis.
    hash = 1469598103934665603ULL;

    // Compute checksum over the dense bin counts.
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }
  }

  return hash;
}
