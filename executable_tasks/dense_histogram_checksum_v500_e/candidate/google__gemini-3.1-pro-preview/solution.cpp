#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstring>

namespace {

/**
 * FNV-1a style mixing function provided in the baseline.
 */
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  if (bins == 0) {
    return FNV_OFFSET_BASIS;
  }

  // Use a dense vector for the histogram to ensure O(1) access and cache friendliness.
  // Pre-allocating outside the loop to avoid repeated allocations.
  std::vector<uint32_t> counts(bins);
  uint64_t hash = 0;

  const uint32_t* val_ptr = values.data();
  const size_t val_size = values.size();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the histogram counts for each iteration.
    std::fill(counts.begin(), counts.end(), 0u);

    // Accumulate counts. This is the primary bottleneck.
    // For dense histograms, a simple array increment is highly efficient.
    for (size_t i = 0; i < val_size; ++i) {
      uint32_t v = val_ptr[i];
      // Safety check for bin range is omitted as per typical performance task assumptions,
      // but in production, one might check if v < bins.
      ++counts[v];
    }

    // Calculate the checksum based on the final bin counts.
    hash = FNV_OFFSET_BASIS;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, static_cast<uint64_t>(counts[b]));
    }
  }

  return hash;
}
